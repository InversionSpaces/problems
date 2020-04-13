#include <functional>
#include <array>
#include <forward_list>
#include <cinttypes>
#include <algorithm>

template<typename T, uint64_t arrsize = 1307>
class HashMap {
	using hashfunc_t = std::function<uint64_t(const T&)>;
	using list_t = std::forward_list<T>;
	using array_t = std::array<list_t, arrsize>;

private:
	array_t arr;
	hashfunc_t hash;
public:
	HashMap(hashfunc_t hashfunc) : hash(hashfunc) {}

	void insert(const T& val) noexcept {
		const uint64_t id = hash(val) % arrsize;
		list_t& list = arr[id];

		auto it = std::find(list.begin(), list.end(), val);
		if (it == list.end())
			list.push_front(val);
	}

	void erase(const T& val) noexcept {
		const uint64_t id = hash(val) % arrsize;
		list_t& list = arr[id];

		list.remove(val);
	}

	bool contains(const T& val) const noexcept {
		const uint64_t id = hash(val) % arrsize;
		const list_t& list = arr[id];
		
		auto it = std::find(list.begin(), list.end(), val);
		return it != list.end();
	}

	uint64_t get_arrsize() const noexcept {
		return arrsize;
	}

	std::array<uint64_t, arrsize> get_stats() {
		std::array<uint64_t, arrsize> retval;
		for (uint64_t i = 0; i < arrsize; ++i)
			retval[i] = std::distance(arr[i].begin(), arr[i].end());

		return retval;
	}
};
