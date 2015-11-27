#include <stdio.h>
#include <floatfann.h>

int main()
{
    fann_type *calc_out;
    fann_type input[4];

    struct fann *ann = fann_create_from_file("pong_float.net");

    input[0] = 0.1;
    input[1] = 0.1;
	input[2] = 0.1;
	input[3] = 0.9;
    calc_out = fann_run(ann, input);



    printf("myPos: %f\ntheirPos: %f\nballX: %f\nballY: %f -> %f\n",
			input[0], input[1], input[2], input[3], calc_out[0]);
	printf("1=move paddle up    -1=movepaddle down\n");

    fann_destroy(ann);
    return 0;
}
