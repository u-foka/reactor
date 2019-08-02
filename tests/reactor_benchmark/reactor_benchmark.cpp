#include <benchmark/benchmark.h>

#include "reactor.hpp"

class i_empty {};
class empty : public i_empty {};

static void BM_Reactor_CreateObj(benchmark::State &state)
{
   iws::reactor::factory_registrator<i_empty, empty, false, true> registrator(iws::reactor::prio_normal);
   iws::reactor::contract<i_empty> contract;

   for (auto _ : state)
   {
      iws::r.reset_objects();
      i_empty &obj = iws::r.get(contract);
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateObj);

static void BM_Reactor_CreateAndAccess(benchmark::State &state)
{
   iws::reactor::factory_registrator<i_empty, empty, false, true> registrator(iws::reactor::prio_normal);
   iws::reactor::contract<i_empty> contract;

   for (auto _ : state)
   {
      iws::r.reset_objects();
      i_empty &created = iws::r.get(contract);
      benchmark::DoNotOptimize(created);
      
      i_empty &obj = iws::r.get(contract);
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateAndAccess);

static void BM_Reactor_CreateAndAccessHolder(benchmark::State &state)
{
   iws::reactor::factory_registrator<i_empty, empty, false, true> registrator(iws::reactor::prio_normal);
   iws::reactor::contract<i_empty> contract;

   for (auto _ : state)
   {
      iws::r.reset_objects();
      i_empty &created = iws::r.get(contract);
      benchmark::DoNotOptimize(created);
      
      std::shared_ptr<i_empty> obj = iws::r.get_ptr(iws::r.get(contract));
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateAndAccessHolder);

BENCHMARK_MAIN();
