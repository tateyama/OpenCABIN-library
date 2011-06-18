#define a(type, name) \
	struct type *name

#define b(name) \
	sizeof(*name)


#include <stdio.h>

struct test {
	int a;
};

int
main(int ac, char **av)
{
	a(test, u);
	struct test v;

	u = &v;

	u->a = 10;

	printf("%d\n", b(u));
	printf("%d\n", v.a);
	return 0;
}
