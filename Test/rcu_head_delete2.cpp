#include <iostream>
#include <unistd.h>
#include "urcu-signal.hpp"
#include "rcu_head_delete2.hpp"

// Derived-type approach.

struct foo: public std::rcu_head_delete2<foo> {
	int a;
};

struct my_deleter {
	void operator () (void*) const { std::cout << "In my_deleter\n"; }
};

template <class T>
using rcu_head_t = std::rcu_head_delete2<T, my_deleter>;

struct bar: public rcu_head_t<bar> {
	int a;
};

int main(int argc, char **argv)
{
	struct bar my_bar;
	struct foo *fp = new struct foo;
	std::rcu_signal rs;

	// First with a normal function.
	fp->a = 42;
	fp->call();
	rcu_barrier(); // Drain all callbacks on general principles

	// Next with a rcu_domain
	fp = new struct foo;
	fp->a = 43;
	fp->call(rs);
	rcu_barrier();

	// Next with my_deleter
	my_bar.a = 44;
	my_bar.call();
	rcu_barrier();


	return 0;
}
