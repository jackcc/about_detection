#include "detect.h"

//#define GPU 

void test_detector(char *name_list, char *cfgfile, char *weightfile, char *filename, float thresh, float hier_thresh, char *outfile, int fullscreen)
{
    char **names = get_labels(name_list);

    image **alphabet = load_alphabet();
    network *net = load_network(cfgfile, weightfile, 0);
    set_batch_network(net, 1);
    srand(2222222);
    double time;
    char buff[256];
    char *input = buff;
    int j;
    float nms=.3;

    //read file form TXT
    FILE *fp;
    char image_file[256];
    if ((fp = fopen(filename, "rt")) == NULL){
          printf("Cannot open file!!\n");
          exit(1);
    }
    while(fgets(image_file, 256, fp) != NULL){ 
          
      	input = image_file;
        if(!input) return;
        strtok(input, "\n");

        image im = load_image_color(input,0,0);
        image sized = letterbox_image(im, net->w, net->h);
        layer l = net->layers[net->n-1];

        box *boxes = calloc(l.w*l.h*l.n, sizeof(box));
        float **probs = calloc(l.w*l.h*l.n, sizeof(float *));
        for(j = 0; j < l.w*l.h*l.n; ++j) probs[j] = calloc(l.classes + 1, sizeof(float *));
        float **masks = 0;
        if (l.coords > 4){
            masks = calloc(l.w*l.h*l.n, sizeof(float*));
            for(j = 0; j < l.w*l.h*l.n; ++j) masks[j] = calloc(l.coords-4, sizeof(float *));
        }

        float *X = sized.data;
        time=what_time_is_it_now();
        network_predict(net, X);
        printf("%s: Predicted in %f seconds.\n", input, what_time_is_it_now()-time);
        get_region_boxes(l, im.w, im.h, net->w, net->h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);
        //if (nms) do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);
        if (nms) do_nms_sort(boxes, probs, l.w*l.h*l.n, l.classes, nms);
        draw_detections(im, l.w*l.h*l.n, thresh, boxes, probs, masks, names, alphabet, l.classes);

        if(outfile){
		char *token, *saveptr;
		char *save_name[10];
		int j;
		for(j = 1; j++; input = NULL){
			token = strtok_r(input, "/", &saveptr);
			if(token == NULL) break;
			//printf("%d: %s\n", j,token);
			save_name[j] = token;
		}

		//printf("save_name: %s\noutfile: %s\n", save_name[j-1],outfile);

		char output_name[256];
		int outfile_index,save_name_index;
		for(outfile_index = 0; outfile_index < strlen(outfile); outfile_index++){
			output_name[outfile_index] = outfile[outfile_index];
		}
		for(save_name_index = 0; save_name_index <= strlen(save_name[j-1]); save_name_index++){
			output_name[outfile_index] = save_name[j-1][save_name_index];
			outfile_index++;
		}
		printf("Saved to:  %s\n",output_name);
        	save_image(im, output_name);
        }
        else{
            save_image(im, "predictions");
#ifdef OPENCV
            cvNamedWindow("predictions", CV_WINDOW_NORMAL); 
            if(fullscreen){
                cvSetWindowProperty("predictions", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
            }
            show_image(im, "predictions");
            cvWaitKey(0);
            cvDestroyAllWindows();
#endif
        }

        free_image(im);
        free_image(sized);
        free(boxes);
        free_ptrs((void **)probs, l.w*l.h*l.n);
        //if (filename) break;
   }
}

int main(int argc, char **argv)
{
    if(argc < 5){
        fprintf(stderr, "usage: %s label_file model_cfg weights txtfile\noption: -thresh: nms threshold\n\t-out dir: save result image to dir\n\t-fullscreen\n", argv[0]);
        return 0;
    }
    gpu_index = find_int_arg(argc, argv, "-i", 0);
    if(find_arg(argc, argv, "-nogpu")) {
        gpu_index = -1;
    }

#ifndef GPU
    gpu_index = -1;
#else
    if(gpu_index >= 0){
        cuda_set_device(gpu_index);
    }
#endif

    float thresh = find_float_arg(argc, argv, "-thresh", .24);
    char *outfile = find_char_arg(argc, argv, "-out", 0);
    int fullscreen = find_arg(argc, argv, "-fullscreen");
    test_detector(argv[1], argv[2], argv[3], argv[4], thresh, .5, outfile, fullscreen);
    return 0;
}

