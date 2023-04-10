/* main.cpp

Copyright 2023 Roger D. Voss

Created by github user roger-dv on 04/08/2023.

Licensed under the MIT License - refer to LICENSE project document.

*/
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>
#include <iostream>

/**
 * Counts the occurrence of string tokens in the collection
 * passed as the input.
 *
 * @tparam C input collection type of string tokens (must
 * support ranges)
 * @tparam T string type of a collection element (must
 * be possible to construct std::string from T)
 * @param collection an input collection of string tokens
 * @return unordered map where key is a string token from
 * the input and its associated value is count of its
 * occurrence in the collection.
 */
template <typename C, typename T = typename C::value_type>
    requires std::ranges::range<C> && std::constructible_from<std::string, T>
auto count_occurrences(const C &collection) {
  std::unordered_map<std::string, unsigned int> counts{};
  counts.reserve(collection.size() * 5 / 3);
  std::ranges::for_each(collection, [&counts](const T& elem) {
    unsigned int new_count = 1;
    if (auto search = counts.find(elem); search != counts.end()) {
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
  { c.emplace_back(std::move(E{})) };
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
 * how to use concepts to constrain aspects of that.
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
  explicit collection_append(C &coll) : collection(coll) {}
  collection_append() = delete;
  collection_append(const collection_append&) = delete;
  collection_append(collection_append&&) = delete;
  ~collection_append() = default;
  collection_append&operator =(const collection_append&) = delete;
  collection_append&operator =(collection_append&&) = delete;

  collection_append& append(T &&item) {
    this->collection.emplace_back(item);
    return *this;
  }

  template<typename R>
    requires MovableRangeElement<R, T>
  void append_range(R &&rng) {
    for(auto &&e : rng) {
      this->collection.emplace_back(e);
    }
  }

  inline iterator begin() noexcept {
    return this->collection.begin();
  }

  inline iterator end() noexcept {
    return this->collection.end();
  }
};

using count_pair_t = std::pair<unsigned int, std::string>;

/**
 * For a collection passed as input, prints out its
 * elements to stdout. The collection element is a
 * tuple consisting of a paired integer and string.
 * Each element is printed on a single line.
 *
 * @tparam C input collection type (must support ranges)
 * @tparam E collection element type - must be a tuple
 * consisting of a paired unsigned integer and string.
 * @param coll input collection
 */
template<typename C, typename E = typename C::value_type>
  requires std::ranges::range<C> && std::same_as<E, count_pair_t>
void print_collection(const C &coll) {
  for (const E &elem : coll) {
    std::cout << elem.first << ": " << elem.second << '\n';
  }
}

int main() {
  // predicate filters for alpha text words and C preprocessor directives that begin with '#'
  // (alpha text string runs also permitted to have hyphen '-' and underscore '_' characters)
  auto const is_alpha_word = [](const std::string_view word) {
    const auto first_char = word.front();
    using elm_t = decltype(first_char);
    auto const other_allowed = first_char == '#' ?
        [](elm_t c) { return false; } : [](elm_t c) { return c == '-' || c == '_'; };
    auto subwrd = word.substr(1);
    for(const auto c : subwrd) {
      if (std::isalpha(c) == 0 && !other_allowed(c)) { return false; }
    }
    return !(std::isalpha(first_char) == 0 && first_char != '#' && first_char != '_');
  };
  // duplicates the input string, makes it lowercase, and returns that as output
  auto const to_lower_case = [](const std::string_view word) {
    std::string word_copy{word};
    std::transform(word_copy.begin(), word_copy.end(), word_copy.begin(), ::tolower);
    return word_copy;
  };
  // the tuple pair pass as input is flipped which is returned as output
  auto const flip_pair = [](const std::pair<std::string, unsigned int> &entry) {
    return std::make_pair(entry.second, entry.first);
  };
  // two tuple pairs as input are compared on their word count value
  auto const compare_counts = [](const count_pair_t &x, const count_pair_t &y) {
    return x.first > y.first;
  };
  // two tuple pairs as input are lexically compared on their word value
  auto const compare_words = [](const count_pair_t &x, const count_pair_t &y) {
    return x.second.compare(y.second) < 0;
  };
  // two tuple pairs as input are compared on their word count values and returns true when they're equal
  auto const found_pred = [](const count_pair_t &arg1, const count_pair_t &arg2) { return arg1.first == arg2.first; };


  std::vector<std::string> words;
  words.reserve(8 * 1024); // preallocate for 8K vector items (vector automatically expands it that is exceeded)
  collection_append add_words{words}; // wrap vector with a custom appender (wrapper class is just for learning)

  // process input from stdin, which will be a stream of text tokens.
  // Text tokens are filtered to alphabetic only and made lower case.
  auto rng0 = std::ranges::istream_view<std::string>{std::cin}
              | std::views::filter(is_alpha_word)
              | std::views::transform(to_lower_case);

  // rng0 range will now be lazy evaluated against the cin input stream
  add_words.append_range(rng0);

  // words collection holds the alpha text tokens read in from stdin;
  // now will count their occurrence in the collection:
  const auto counts_map = count_occurrences(words);

  // the map of word counts, where map key is the word and its value its count,
  // is transferred to a vector of tuple pair where the first tuple item is the
  // word count and the second tuple item is the word; the vector is then sorted
  std::vector<count_pair_t> count_pairs{};
  count_pairs.reserve(counts_map.size());
  std::set<unsigned int> just_counts{};
  auto const collect_just_counts = [counts=&just_counts]
      (const count_pair_t &item) { counts->emplace(item.first); return true; };
  auto rng1 = counts_map
              | std::views::transform(flip_pair)
              | std::views::filter(collect_just_counts);
  // rng1 range will now be lazy evaluated as its elements are move appended to count_pairs
  for(auto &&count_pair : rng1) {
    count_pairs.emplace_back(count_pair);
  }
  // sort by word count
  std::ranges::sort(count_pairs, compare_counts);

  // make order of word counts descend from largest to smallest
  // (retain the returned range to iterate against)
  auto just_counts_rng = std::views::reverse(just_counts);

  // for each unique count of words find the set of words that have that count and sort that subset range
  std::array<count_pair_t, 1> the_search_item{ count_pair_t{ 0, std::string{} } };
  auto current = count_pairs.begin();
  const auto very_end = count_pairs.end();
  int check_count = 0, sub_rng_count = 0;
  for(const auto n : just_counts_rng) {
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

  // debug (to stderr)
  std::cerr << "\nDEBUG: word-count-set: { ";
  std::ranges::for_each(just_counts_rng, [](const auto &n) { std::cerr << n << ' '; });
  std::cerr << "}\n";

  fprintf(stderr, "\nDEBUG: word-count-set size: %lu, check count: %d, sub-range count: %d\n\n",
          just_counts.size(), check_count, sub_rng_count);


  // main output (to stdout)
  print_collection(count_pairs);


  // reduce to sorted, unique set of words that were counted
  std::ranges::sort(words);
  auto rng2 = std::ranges::unique(words);
  words.erase(rng2.begin(), rng2.end());

  std::cerr << "\nDEBUG: counted words:\n";
  std::ranges::copy(words, std::ostream_iterator<std::string>(std::cerr, "\n"));
}