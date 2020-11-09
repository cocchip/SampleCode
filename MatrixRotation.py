#!/bin/python3

import math
import os
import random
import re
import sys

#######################################################################################################
# Gather basic information on the rectangle we will process, including the "shorter" axis, and how many
# rectangles deep it is.
def determine_box_parameters(axis, m, n):
    #The axis is guaranteed to be <= the other element, and also divisible by 2
    
    rectangles = int()
    other_axis = int()
    other_axis_remainder = int()
    if axis == "m":
        rectangles = int(m/2)
        other_axis = int(n/2)
        other_axis_remainder = int(n%2)
    else:
        rectangles = int(n/2)
        other_axis = int(m/2)
        other_axis_remainder = int(m%2)
    
    return(rectangles, other_axis, other_axis_remainder)

#######################################################################################################
#Find the next position in the rectangle rotation, if given the current position
def next_position(prev_x, prev_y, width, height, offset):
    x = prev_x
    y = prev_y

    if (width == 1 and height == 1):    #special case, we are in the center (this should not happen given min(m, n)%2 = 0)
        return(x, y)
    
    if y +1 < (height + offset) and x == (0 + offset):
        y+=1                #traverse left edge down
    elif y+1 == (height + offset) and x+1 < (width +offset):
        x+=1                #traverse bottom edge right
    elif y >(0+offset) and x+1 == (width + offset):
        y-=1                #traverse right edge up
    elif y== (0+offset) and x>(0+ offset):
        x-=1                #traverse top edge left
    else:
        print("Bad input parameters, we are not at an edge\n")   #should not happen with good input data. We should always be at an edge
                
    return(x, y)

    
#######################################################################################################
#Read on rectangle into a string (starting at rotated position.)
def unpack_rectangle(matrix,rectangle_length, width, height, rotation, current_rectangle):
    done = 0
    out_list = list()
    
    if rotation == 0:
        mod_rotation = 0
    else:
        mod_rotation = rectangle_length - rotation #rotation inverted since rotation function rotates in the other dirction, we just rotate the initial rotation "mod" times
        
    #rotate to get in position for this rectangle
    x=current_rectangle
    y=current_rectangle
    while done < mod_rotation:
        x, y = next_position(x, y, width, height, current_rectangle)
        done +=1
    
    #now read it into the string    
    done = 0
    while done < rectangle_length:
        out_list.append( matrix[y][x])
        x, y = next_position(x, y, width, height, current_rectangle)
        done +=1

    return(out_list)

#######################################################################################################
#Return values are NOT X&Y. The first return value is the number of concentric rectangles we are in.
#The second return value is the offset into the concentric rectangle string (we build earlier) to read from
def which_element_to_print(num_rectangles, width, height, i, j):
    which_rectangle = 0
    location = 0
    
    #offset is another way of saying how many levels of concentric rectangles are we inside
    #it is defined as the minimum distance between the location and any edge
    offset = min(i, j, height-1-i, width-1-j)
    
    #when comparing against the edge, we use offset (since inner rectangle is moved one line down or right from the edge)
    if (i==offset and j==offset):                   #top left corner
        return(offset, 0)
    elif (i==offset):                               #top row
        return(offset, -j+offset)
    elif (i+offset==height-1):                      #bottom row
        return(offset, j -1 + height -(offset *3))
    elif (j==offset and i>offset and i+(offset)< height-1):    #left edge
        return(offset,i-offset)
    elif (j==width-1-offset and i>offset and i< height-1-offset):    #right edge
        return(offset,-width-i+1+(3*offset))
    else:
        return(0,0)                                    #should not occur                        
            
    return(which_rectangle, location)                  #should not be reached
    
#######################################################################################################
def print_boxes(num_rectangles, maxtrix, rectangle_strings, width, height):
    
    for i in range(height):
        for j in range(width):
            which_rectangle, location = which_element_to_print(num_rectangles, width, height, i, j)
            
            print(rectangle_strings[which_rectangle][location], end=" ")
        print("")
            
#######################################################################################################
# Complete the matrixRotation function below.
def matrixRotation(matrix, r, m, n):

    #first we are determining the rotation element
    axis=""
    if m < n:
        axis = "m"
    else:
        axis = "n"
    num_rectangles, other_axis, other_axis_remainder = determine_box_parameters(axis, m,n)

    rectangle_strings = list()
    height = m
    width = n
    for i in range(num_rectangles):
        rectangle_length = (2*width) + (2*height) - 4    #-4 because we don't want to count the corners twice.
        full_string = list()
        rotation = r % rectangle_length                  #mod here, because we don't need to include full rotations
        
        #Unstrip each concentric rectangle into a string. We also rotate around this string. We will print from this string.
        rectangle_strings.append(unpack_rectangle(matrix, rectangle_length, width, height, rotation, i))        
        width -= 2
        height -=2
    
    print_boxes(num_rectangles, matrix, rectangle_strings, n, m)

#######################################################################################################

if __name__ == '__main__':
    mnr = input().rstrip().split()

    m = int(mnr[0])

    n = int(mnr[1])

    r = int(mnr[2])

    matrix = []

    for _ in range(m):
        matrix.append(list(map(int, input().rstrip().split())))

    matrixRotation(matrix, r, m, n)
