
#include <std/iterator.hpp>
#include <std/range.hpp> // FIXME: Wrong test.

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


// FIXME: Remove this.
template<stl::Range R>
  requires stl::Sortable<stl::iterator_t<R>>()
void sort(R const& range) { }


int main()
{
  std::list<int> lst;
  sort(lst);
}
