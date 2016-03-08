
#include <std/functional.hpp>

#include <cassert>
#include <string>
#include <memory>



int main()
{
  std::less<>    less1;
  std::less<int> less2;
  assert(less1(1, 2));
  assert(less2(1, 2));
}
