#include <iostream>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <vector>
#include <set>
//#include <format>

template <typename C, typename T = typename C::value_type>
    requires std::ranges::range<C> && std::constructible_from<std::string, T>
auto count_occurences(const C &collection) {
  std::unordered_map<std::string, unsigned int> counts{};
  counts.reserve(collection.size() * 5 / 3);
  std::ranges::for_each(collection, [&counts](const T& elem) {
    unsigned int new_count = 1;
    auto search = counts.find(elem);
    if (search != counts.end()) {
      const auto current_count = search->second;
      new_count += current_count;
    }
    counts[elem] = new_count;
  });
  return counts;
}

template<typename C, typename E = typename C::value_type>
concept AppendableCollection = requires(C c) {
  { &c } -> std::input_iterator;
  requires std::move_constructible<E>;
  { c.emplace(c.end(), std::move(E{})) };
  { c.insert(c.begin(), c.begin(), c.end()) };
};

template<typename R, typename T>
concept MovableRangeElement = requires(R r) {
  std::ranges::begin(r);
  std::ranges::end(r);
  { *(r.begin()) } -> std::same_as<T>;
  requires std::move_constructible<T>;
};

/**
 * This template class is just an exercise in writing
 * a class that has some behaviors of a collection and
 * how to used concepts to constrain aspects of that.
 *
 * An instantiation of this template will wrap the
 * actual underlying collection.
 *
 * IOW, in the word counting code below, this class
 * could be dispensed with and the underlying collection
 * used directly instead. Again, is for learning.
 *
 * @tparam C type of the collection being wrapped
 * @tparam T type of a collection element
 * @tparam CIter constant iterator type
 * @tparam Iter non constant iterator type
 */
template <typename C,
          typename T = typename C::value_type,
          typename CIter = typename C::const_iterator,
          typename Iter = typename C::iterator>
  requires AppendableCollection<C>
class collection_append {
private: C &collection;
public:
  using value_type [[maybe_unused]] = T;
  using const_iterator [[maybe_unused]] = CIter;
  using iterator = Iter;
  explicit collection_append(C &a_collection) : collection(a_collection) {}
  collection_append() = delete;
  collection_append(const collection_append&) = delete;
  collection_append(collection_append&&) = delete;
  ~collection_append() = default;
  collection_append&operator =(const collection_append&) = delete;
  collection_append&operator =(collection_append&&) = delete;

  collection_append& append(T &&item) {
    this->collection.emplace(this->collection.end(), std::move(item));
    return *this;
  }

  template<typename R>
    requires MovableRangeElement<R, T>
  void insert([[maybe_unused]]/*quite bogus warning*/ const_iterator pos,
              [[maybe_unused]]/*quite bogus warning*/ R &&r)
  {
    for(auto &&e : r) {
      this->collection.insert(pos++, e);
    }
  }

  inline iterator begin() const noexcept {
    return this->collection.begin();
  }

  inline iterator end() const noexcept {
    return this->collection.end();
  }
};

template<typename C, typename E = typename C::value_type>
  requires std::ranges::range<C>
void print_collection([[maybe_unused]]/*quite bogus warning*/ const C &collection) {
  for (const E &elem : collection) {
//    std::cout << std::format("{}: {}\n", elem.first, elem.second);
    printf("%d: %s\n", elem.first, elem.second.c_str());
  }
}


int main() {
  auto const is_alpha_word = [](const std::string_view word) {
    for(const char c : word) {
      if (std::isalpha(c) == 0) { return false; }
    }
    return true;
  };
  auto const to_lower_case = [](const std::string_view word) {
    std::string word_copy{word};
    std::transform(word_copy.begin(), word_copy.end(), word_copy.begin(), ::tolower);
    return word_copy;
  };
  auto const flip_pair = [](const std::pair<std::string, unsigned int> &entry) {
    return std::make_pair(entry.second, entry.first);
  };
  using count_pair_t = std::pair<unsigned int, std::string>;
  auto const compare_counts = [](const count_pair_t &x, const count_pair_t &y) {
    return x.first > y.first;
  };
  auto const compare_words = [](const count_pair_t &x, const count_pair_t &y) {
    return x.second.compare(y.second) < 0;
  };
  auto const found_pred = [](const count_pair_t &arg1, const count_pair_t &arg2) { return arg1.first == arg2.first; };


  std::vector<std::string> words;
  words.reserve(64 * 1024); // preallocate 64K vector
  collection_append add_words{words}; // wrap vector with a custom appender

  auto rng0 = std::ranges::istream_view<std::string>{std::cin}
              | std::views::filter(is_alpha_word)
              | std::views::transform(to_lower_case);

  // rng0 range will now be lazy evaluated against the cin input stream
  add_words.insert(add_words.end(), rng0);

  const auto counts_map = count_occurences(words);

  std::vector<count_pair_t> count_pairs;
  count_pairs.reserve(counts_map.size());
  std::set<unsigned int> just_counts;
  auto const collect_just_counts = [&just_counts](const count_pair_t &item) { just_counts.emplace(item.first); return true; };
  auto rng1 = counts_map
              | std::views::transform(flip_pair)
              | std::views::filter(collect_just_counts);
  // rng1 range will now be lazy evaluated as its elements are inserted into count_pairs
  count_pairs.insert(count_pairs.end(), std::ranges::begin(rng1), std::ranges::end(rng1));
  // sort by word count
  std::ranges::sort(count_pairs, compare_counts);

  auto just_counts_rng = std::views::reverse(just_counts);

  std::array<count_pair_t, 1> the_search_item{ count_pair_t{0, std::string{}} };
  auto current = count_pairs.begin();
  const auto very_end = count_pairs.end();
  int check_count = 0, sub_rng_count = 0;
  for(const unsigned int n : just_counts_rng) {
    check_count++;
    the_search_item[0].first = n;
    current = std::find_first_of(current, very_end, the_search_item.begin(), the_search_item.end(), found_pred);
    if (current != very_end) {
      auto sub_rng_end = std::find_end(current, very_end, the_search_item.begin(), the_search_item.end(), found_pred);
      auto sub_rng = std::ranges::subrange(current, sub_rng_end != very_end ? sub_rng_end + 1 : sub_rng_end);
      std::ranges::sort(sub_rng, compare_words);
      if (sub_rng_end != very_end) {
        current = sub_rng_end;
      }
      sub_rng_count++;
    } else {
      break;
    }
  }



  fputs("\nDEBUG: ", stderr);
  for(const unsigned int n : just_counts_rng) {
    fprintf(stderr, "%d, ", n);
  }
  putc('\n', stderr);

  fprintf(stderr, "\nDEBUG: set count: %lu, check count: %d, sub range count: %d\n\n",
          just_counts.size(), check_count, sub_rng_count);


  // main output
  print_collection(count_pairs);


  std::ranges::sort(words);
  auto rngs2 = std::ranges::unique(words);
  fputs("\nDEBUG:\n", stderr);
  std::ranges::copy(rngs2, std::ostream_iterator<std::string>(std::cerr, "\n"));
}