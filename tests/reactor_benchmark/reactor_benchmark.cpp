#include <benchmark/benchmark.h>

#include <reactor/client.hpp>
#include <reactor/provider.hpp>

using namespace iws::reactor;

class i_empty {};
class empty : public i_empty {};

static void BM_Reactor_CreateObj(benchmark::State &state)
{
   factory_registrator<i_empty, empty, false, true> registrator(prio_normal);
   contract<i_empty> contract;

   for (auto _ : state)
   {
      r.reset_objects();
      i_empty &obj = r.get(contract);
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateObj);

static void BM_Reactor_CreateAndAccess(benchmark::State &state)
{
   factory_registrator<i_empty, empty, false, true> registrator(prio_normal);
   contract<i_empty> contract;

   for (auto _ : state)
   {
      r.reset_objects();
      i_empty &created = r.get(contract);
      benchmark::DoNotOptimize(created);
      
      i_empty &obj = r.get(contract);
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateAndAccess);

static void BM_Reactor_CreateAndAccessHolder(benchmark::State &state)
{
   factory_registrator<i_empty, empty, false, true> registrator(prio_normal);
   contract<i_empty> contract;

   for (auto _ : state)
   {
      r.reset_objects();
      i_empty &created = r.get(contract);
      benchmark::DoNotOptimize(created);
      
      std::shared_ptr<i_empty> obj = r.get_ptr(r.get(contract));
      benchmark::DoNotOptimize(obj);
   }
}
BENCHMARK(BM_Reactor_CreateAndAccessHolder);

BENCHMARK_MAIN();
