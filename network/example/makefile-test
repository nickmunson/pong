
execute:
	clang -L/usr/local/lib -I/usr/local/include -lfann execute.c -o execute
	./execute

train:
	clang -L/usr/local/lib -I/usr/local/include -lfann train.c -o train
	./train

clean:
	rm train execute
