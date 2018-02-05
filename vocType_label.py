import xml.etree.ElementTree as ET
import pickle
import os
from os import listdir, getcwd
from os.path import join
import sys
import argparse
import pdb

classes = ["bicycle", "bus", "car", "motorbike"] 

def convert(size, box):
    dw = 1./size[0]
    dh = 1./size[1]
    x = (box[0] + box[1])/2.0
    y = (box[2] + box[3])/2.0
    w = box[1] - box[0]
    h = box[3] - box[2]
    x = x*dw
    w = w*dw
    y = y*dh
    h = h*dh
    return (x,y,w,h)

def convert_annotation(input_dir,output_dir, image_id):
    in_file = open('%s/%s'%(input_dir, image_id))
    out_file = open('%s/labels/%s.txt'%(output_dir, os.path.splitext(image_id)[0]), 'w')
    tree=ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    w = int(size.find('width').text)
    h = int(size.find('height').text)

    for obj in root.iter('object'):
        difficult = obj.find('difficult').text
        cls = obj.find('name').text
        if cls not in classes or int(difficult) == 1:
            continue
        cls_id = classes.index(cls)
        xmlbox = obj.find('bndbox')
        b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text), float(xmlbox.find('ymax').text))
        bb = convert((w,h), b)
        out_file.write(str(cls_id) + " " + " ".join([str(a) for a in bb]) + '\n')


def main(args):

    input_dir = os.path.expanduser(args.input_dir)
    if not os.path.exists(input_dir):
        print("%s is not exists"%args.input_dir)
        return 
    output_dir = os.path.expanduser(args.output_dir)
    if not os.path.exists(output_dir):
        os.path.mkdir(output_dir)
    #pdb.set_trace()
    image_ids = os.listdir(input_dir)
    nrof_ids = len(image_ids)
    if nrof_ids < 1 :
        print("%s has no xml file"%input_dir)
        return

    #list_file = os.path.join(getcwd(),args.output_filename)     
    #with open(list_file, "w") as text_file:
    list_file = open('list.txt', 'w')
    if not os.path.exists('%s/labels/'%(output_dir)):
        os.makedirs('%s/labels/'%(output_dir))
    for image_id in image_ids:
        list_file.write('%s.jpg\n'%(os.path.splitext(image_id)[0]))
        convert_annotation(input_dir, output_dir, image_id)
    list_file.close()
    os.system("sed -i s#^#/mnt/fullImgFile/Done/bus/#g list.txt")



def parse_arguments(argv):
    parser = argparse.ArgumentParser()
        
    parser.add_argument('input_dir', type=str, help='Directory with XML.')
    parser.add_argument('output_dir', type=str, help='Directory with LABEL.')
    #parser.add_argument('output_filename', type=str, default="list.txt", help='File with image list info.')
    return parser.parse_args(argv)

if __name__ == '__main__':
    main(parse_arguments(sys.argv[1:]))
