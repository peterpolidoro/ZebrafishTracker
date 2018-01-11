import cv2
import numpy as np
import argparse
import os


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("images_path", help="Path to images.")

    args = parser.parse_args()
    images_path = args.images_path
    print("Images Path: {0}".format(images_path))

    background = cv2.imread(os.path.join(images_path,"background.png"),cv2.IMREAD_GRAYSCALE)
    cv2.imshow('background',background)
    cv2.waitKey(0)
    cv2.destroyWindow('background')

    fish01 = cv2.imread(os.path.join(images_path,"fish01.png"),cv2.IMREAD_GRAYSCALE)

    foreground = cv2.subtract(background,fish01)
    cv2.imshow('foreground',foreground)
    cv2.waitKey(0)
    cv2.destroyWindow('foreground')

    (retval,threshold) = cv2.threshold(foreground,25,255,cv2.THRESH_BINARY)
    cv2.imshow('threshold',threshold)
    cv2.waitKey(0)
    cv2.destroyWindow('threshold')

    kernel = np.ones((3,3),np.uint8)
    erosion = cv2.erode(threshold,kernel,iterations = 1)
    cv2.imshow('erosion',erosion)
    cv2.waitKey(0)
    cv2.destroyWindow('erosion')
