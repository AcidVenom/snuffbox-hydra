#include <foundation/auxiliary/logger.h>
#include <foundation/memory/memory.h>

using namespace snuffbox;
using namespace foundation;

class Test
{

public:

  Test(int v) :
    test_(v)
  {

  }

  int test() const
  {
    return test_;
  }

  ~Test()
  {
    Logger::Log(Logger::Channel::kPlayer, Logger::Verbosity::kDebug, "Destructed..");
  }

private:

  int test_;
};

int main(int argc, char** argv)
{
  Test* test = Memory::Construct<Test>(1000);
  Logger::Log(Logger::Channel::kPlayer, Logger::Verbosity::kDebug, "{0}", test->test());
  Memory::Destruct(test);

  std::cin.get();
  return 0;
}