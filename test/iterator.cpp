
#include <std/iterator.hpp>

#include <forward_list>
#include <list>
#include <vector>


using fiter = std::forward_list<int>::iterator;
using biter = std::list<int>::iterator;
using riter = std::vector<int>::iterator;

template<stl::ForwardIterator I>
constexpr bool test_forward_iterator() { return true; }

template<stl::BidirectionalIterator I>
constexpr bool test_bidirectional_iterator() { return true; }

template<stl::RandomAccessIterator I>
constexpr bool test_random_access_iterator() { return true; }

static_assert(test_forward_iterator<fiter>());
static_assert(test_bidirectional_iterator<biter>());
static_assert(test_random_access_iterator<riter>());


int main()
{
}
