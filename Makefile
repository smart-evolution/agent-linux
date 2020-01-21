all: 
	gcc -I ../../coda-it/cutillib main.c -o main

.PHONY: fix
fix:
	clang-format -i -style=file *.c *.h

.PHONY: id
id:
	./scripts/id_generator.sh

.PHONY: version
version:
	git tag $(V)
	./scripts/changelog.sh
	git add ./docs/changelogs/CHANGELOG_$(V).md
	./scripts/version.sh $(V)
	git add ./version.h
	git commit --allow-empty -m "Build $(V)"
	git tag --delete $(V)
	git tag $(V)	