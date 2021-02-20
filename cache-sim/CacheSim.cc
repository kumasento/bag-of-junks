#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/// ---------------------- Memory Trace ----------------------------------------
struct Access {
  bool isLoad;
  uint32_t addr;

  Access(bool isLoad, uint32_t addr) : isLoad(isLoad), addr(addr) {}
};

/// ---------------------- Cache Model -----------------------------------------

class Cache {
public:
  using AddrT = uint32_t;

  struct Config {
    const size_t numSets;
    const size_t numBlocksPerSet;
    const size_t numBytesPerBlock;
    const size_t numAddressBits;
    const size_t numSetBits;
    const size_t numTagBits;
    const size_t numOffsetBits;

    const std::string writeMissPolicy;
    const std::string writeHitPolicy;
    const std::string lineReplacementPolicy;

    Config(const size_t numSets, const size_t numBlocksPerSet,
           const size_t numBytesPerBlock, const size_t numAddressBits,
           const std::string &writeMissPolicy,
           const std::string &writeHitPolicy,
           const std::string &lineReplacementPolicy)
        : numSets(numSets), numBlocksPerSet(numBlocksPerSet),
          numBytesPerBlock(numBytesPerBlock), numAddressBits(numAddressBits),
          writeMissPolicy(writeMissPolicy), writeHitPolicy(writeHitPolicy),
          lineReplacementPolicy(lineReplacementPolicy),
          numSetBits(std::bitset<32>(numSets - 1).count()),
          numOffsetBits(std::bitset<32>(numBytesPerBlock - 1).count()),
          numTagBits(numAddressBits - std::bitset<32>(numSets - 1).count() -
                     std::bitset<32>(numBytesPerBlock - 1).count()) {}

    void print() const {
      printf("\n");
      printf("Cache configuration\n");
      printf("--------------------------------------\n");
      printf("Num cache sets:            %lu (%lu)\n", numSets, numSetBits);
      printf("Num cache blocks per set:  %lu\n", numBlocksPerSet);
      printf("Num cache bytes per block: %lu (%lu)\n", numBytesPerBlock,
             numOffsetBits);
      printf("Num memory address bits:   %lu\n", numAddressBits);
      printf("Num tag bits:              %lu\n", numTagBits);
      printf("Write miss policy:         %s\n", writeMissPolicy.c_str());
      printf("Write hit policy:          %s\n", writeHitPolicy.c_str());
      printf("Line replacement policy:   %s\n", lineReplacementPolicy.c_str());
      printf("\n");
    }
  };

  struct Record {
    bool hit;
    size_t numCycles;

    Record() : hit(false), numCycles(0) {}
  };

  /// Don't hold the actual data in the cache line.
  struct Line {
    bool valid;
    bool dirty; // for write-back policy
    AddrT tag;
    size_t timestamp;

    Line() : valid(false), dirty(false), tag(0) {}
    Line(bool valid, AddrT tag) : valid(valid), tag(tag), dirty(false) {}
  };

  using Set = std::vector<Line>;

  explicit Cache(Config config) : config(config) {}
  virtual ~Cache() {}

  virtual Record read(AddrT addr) = 0;
  virtual Record write(AddrT addr) = 0;

protected:
  Config config;

  AddrT getTag(AddrT addr) const {
    return addr >> (config.numSetBits + config.numOffsetBits);
  }

  AddrT getSet(AddrT addr) const {
    return (addr >> config.numOffsetBits) & (config.numSets - 1);
  }

  AddrT getOffset(AddrT addr) const {
    return addr & (config.numBytesPerBlock - 1);
  }
};

class DirectMappedCache : public Cache {
public:
  DirectMappedCache(Config config) : Cache(config) {
    lines.resize(config.numSets);
  }

  Record read(AddrT addr) {
    Record record;
    AddrT set = getSet(addr);
    AddrT tag = getTag(addr);

    if (lines[set].valid && lines[set].tag == tag) {
      record.hit = true;
      // Only takes one cycle to read from the cache.
      record.numCycles = 1;
    } else {
      // A cache read miss.
      lines[set].valid = true;
      lines[set].tag = tag;

      // Cycle counts to read from the memory.
      record.numCycles = 100 * (config.numBytesPerBlock >> 2);
      // Cycle counts to write back to the memory for the evicted block.
      if (config.writeHitPolicy == "write-back" && lines[set].dirty) {
        // Reset the dirty bit.
        lines[set].dirty = false;
        record.numCycles += 100 * (config.numBytesPerBlock >> 2);
      }
    }

    return record;
  }

  Record write(AddrT addr) {
    Record record;
    AddrT set = getSet(addr);
    AddrT tag = getTag(addr);

    if (lines[set].valid && lines[set].tag == tag) {
      record.hit = true;
      if (config.writeHitPolicy == "write-through") {
        // immediately write the 4 bytes back to the memory.
        record.numCycles = 1 + 100;
      } else if (config.writeHitPolicy == "write-back") {
        lines[set].dirty = true;
        record.numCycles = 1;
      }
    } else {
      if (config.writeMissPolicy == "write-allocate") {
        lines[set].tag = tag;
        lines[set].valid = true;

        // Read into cache and write to it.
        record.numCycles = 1 + 100 * (config.numBytesPerBlock >> 2);
        if (config.writeHitPolicy == "write-back" && lines[set].dirty) {
          record.numCycles += 100 * (config.numBytesPerBlock >> 2);
        }

        // No matter what the dirty bit was, now it should be true.
        lines[set].dirty = true;
      } else if (config.writeMissPolicy == "no-write-allocate") {
        // Just write to the memory.
        record.numCycles = 100;
      }
    }

    return record;
  }

private:
  std::vector<Line> lines;
};

class SetAssocCache : public Cache {
public:
  SetAssocCache(Config config) : Cache(config), timestamp(0) {
    sets.resize(config.numSets);
  }

  Record read(AddrT addr) {
    Record record;
    AddrT set = getSet(addr);
    AddrT tag = getTag(addr);

    // If the tag can be found in the given set and the valid bit is set.
    auto it = std::find_if(sets[set].begin(), sets[set].end(),
                           [&](const Line &line) { return line.tag == tag; });
    if (it != sets[set].end() && it->valid) {
      record.hit = true;
      record.numCycles = 1;
    } else {
      // If there are empty lines.
      Line newLine(true, tag);
      if (sets[set].size() < config.numBlocksPerSet) {
        // No need to set tag.
        // No eviction. Just read from the memory.
        record.numCycles = 100 * (config.numBytesPerBlock >> 2);
      } else {
        // Need line replacement.
        Set::iterator lineToReplace;
        if (config.lineReplacementPolicy == "lru") {
          lineToReplace = std::min_element(sets[set].begin(), sets[set].end(),
                                           [](const Line &a, const Line &b) {
                                             return a.timestamp < b.timestamp;
                                           });
        } else if (config.lineReplacementPolicy == "fifo") {
          lineToReplace = sets[set].begin();
        }

        // If the original line is dirty.
        bool dirty = lineToReplace->dirty;
        sets[set].erase(lineToReplace);

        // Place the new line.
        Line newLine(true, tag);

        // Read from the memory.
        record.numCycles = 100 * (config.numBytesPerBlock >> 2);
        if (config.writeHitPolicy == "write-back" && dirty)
          record.numCycles += 100 * (config.numBytesPerBlock >> 2);
      }

      sets[set].push_back(newLine);
      it = std::prev(sets[set].end());
    }

    it->timestamp = timestamp;
    // NOTE: maybe the timestamp should be the system time instead of a counter?
    timestamp++;

    return record;
  }

  Record write(AddrT addr) {
    Record record;
    AddrT set = getSet(addr);
    AddrT tag = getTag(addr);

    // If the tag can be found in the given set and the valid bit is set.
    auto it = std::find_if(sets[set].begin(), sets[set].end(),
                           [&](const Line &line) { return line.tag == tag; });
    if (it != sets[set].end() && it->valid) {
      record.hit = true;
      record.numCycles = 1;

      if (config.writeHitPolicy == "write-through") {
        // immediately write the 4 bytes back to the memory.
        record.numCycles = 1 + 100;
      } else if (config.writeHitPolicy == "write-back") {
        it->dirty = true;
        record.numCycles = 1;
      }
    } else {
      if (config.writeMissPolicy == "write-allocate") {
        // If there are empty lines.
        Line newLine(true, tag);

        // If there are empty lines.
        if (sets[set].size() < config.numBlocksPerSet) {
          // No eviction. Just read from the memory.
          record.numCycles = 100 * (config.numBytesPerBlock >> 2);
        } else {
          // Need line replacement.
          Set::iterator lineToReplace;
          if (config.lineReplacementPolicy == "lru") {
            lineToReplace = std::min_element(sets[set].begin(), sets[set].end(),
                                             [](const Line &a, const Line &b) {
                                               return a.timestamp < b.timestamp;
                                             });
          } else if (config.lineReplacementPolicy == "fifo") {
            lineToReplace = sets[set].begin();
          }

          // If the original line is dirty.
          bool dirty = lineToReplace->dirty;
          sets[set].erase(lineToReplace);

          // Read from the memory.
          record.numCycles = 100 * (config.numBytesPerBlock >> 2);
          if (config.writeHitPolicy == "write-back" && dirty)
            record.numCycles += 1 + 100 * (config.numBytesPerBlock >> 2);
        }

        sets[set].push_back(newLine);
        it = std::prev(sets[set].end());
        it->timestamp = timestamp;
      } else if (config.writeMissPolicy == "no-write-allocate") {
        // Just write to the memory.
        // Timestamp won't be updated since the cache is bypassed.
        record.numCycles = 100;
      }
    }

    timestamp++;
    return record;
  }

private:
  size_t timestamp;
  std::vector<Set> sets;
};

std::unique_ptr<Cache> CreateCache(Cache::Config config) {
  if (config.numBlocksPerSet == 1)
    return std::make_unique<DirectMappedCache>(config);
  else if (config.numSets > 1)
    return std::make_unique<SetAssocCache>(config);

  return nullptr;
}

/// ---------------------- Simulation ----------------------------------------
class Simulator {
public:
  struct Result {
    size_t numReads, numWrites, numReadHits, numReadMisses, numWriteHits,
        numWriteMisses, numCycles;

    Result()
        : numReads(0), numWrites(0), numReadHits(0), numReadMisses(0),
          numWriteHits(0), numWriteMisses(0), numCycles(0) {}
  };

  Result simulate(std::unique_ptr<Cache> &cache,
                  const std::vector<Access> &traces) {
    Result result;

    for (const auto &trace : traces) {
      Cache::Record record =
          trace.isLoad ? cache->read(trace.addr) : cache->write(trace.addr);

      if (trace.isLoad) {
        result.numReads++;

        if (record.hit)
          result.numReadHits++;
        else
          result.numReadMisses++;
      } else {
        result.numWrites++;

        if (record.hit)
          result.numWriteHits++;
        else
          result.numWriteMisses++;
      }

      result.numCycles += record.numCycles;
    }

    return result;
  }
};

int main(int argc, char *argv[]) {
  // Read the memory traces from the stdin.
  char accessType;
  uint32_t accessAddr;
  int unknownField;
  std::vector<Access> traces;
  while (scanf("%c 0x%x %d", &accessType, &accessAddr, &unknownField) == 3) {
    traces.push_back(Access(accessType == 'l', accessAddr));
    getchar(); // capture the newline character.
  }

  printf("\n");
  printf("Memory trace file\n");
  printf("--------------------------------------\n");
  printf("Number of memory accesses: %lu\n", traces.size());
  printf("\n");

  // Configure the cache.
  assert(argc == 7 && "There should be 7 arguments");
  const int numSets = atoi(argv[1]);
  const int numBlocksPerSet = atoi(argv[2]);
  const int numBytesPerBlock = atoi(argv[3]);
  const int numAddressBits = 32;
  const std::string writeMissPolicy(argv[4]);
  const std::string writeHitPolicy(argv[5]);
  const std::string lineReplacementPolicy(argv[6]);

  Cache::Config config(numSets, numBlocksPerSet, numBytesPerBlock,
                       numAddressBits, writeMissPolicy, writeHitPolicy,
                       lineReplacementPolicy);
  config.print();

  // Create cache.
  std::unique_ptr<Cache> cache = CreateCache(config);
  assert(cache != nullptr && "Cache has not been constructed.");

  // Simulate cache behaviour.
  Simulator sim;
  Simulator::Result result = sim.simulate(cache, traces);

  printf("\n");
  printf("Simulation result\n");
  printf("--------------------------------------\n");
  printf("Total loads:      %lu\n", result.numReads);
  printf("Total stores:     %lu\n", result.numWrites);
  printf("Load hits:        %lu\n", result.numReadHits);
  printf("Load misses:      %lu\n", result.numReadMisses);
  printf("Store hits:       %lu\n", result.numWriteHits);
  printf("Store misses:     %lu\n", result.numWriteMisses);
  printf("Total cycles:     %lu\n", result.numCycles);
  printf("\n");

  return 0;
}
