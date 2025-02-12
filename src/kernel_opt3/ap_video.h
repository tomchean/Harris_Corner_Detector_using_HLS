/*******************************************************************************
Vendor: Xilinx
Associated Filename: ap_video.h
Purpose: Video datatype header file for AutoESL
Revision History: February 13, 2012 - initial release

*******************************************************************************
© Copyright 2008 - 2012 Xilinx, Inc. All rights reserved.

This file contains confidential and proprietary information of Xilinx, Inc. and
is protected under U.S. and international copyright and other intellectual
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials
distributed herewith. Except as otherwise provided in a valid license issued to
you by Xilinx, and to the maximum extent permitted by applicable law:
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether
in contract or tort, including negligence, or under any other theory of
liability) for any loss or damage of any kind or nature related to, arising under
or in connection with these materials, including for any direct, or any indirect,
special, incidental, or consequential loss or damage (including loss of data,
profits, goodwill, or any type of loss or damage suffered as a result of any
action brought by a third party) even if such damage or loss was reasonably
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any
application requiring fail-safe performance, such as life-support or safety
devices or systems, Class III medical devices, nuclear facilities, applications
related to the deployment of airbags, or any other applications that could lead
to death, personal injury, or severe property or environmental damage
(individually and collectively, "Critical Applications"). Customer assumes the
sole risk and liability of any use of Xilinx products in Critical Applications,
subject only to applicable laws and regulations governing limitations on product
liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
ALL TIMES.

*******************************************************************************/

#ifndef ___AP__VIDEO__
#define ___AP__VIDEO__

#include "ap_int.h"


/* Parametrized RGB structure */
template <int A, int D, int C>
  struct ap_rgb{
    ap_uint<A> B;
    ap_uint<D> G;
    ap_uint<C> R;
  };

/* Parametrized YUV structure */
template <int A, int B, int C>
  struct ap_yuv{
    ap_uint<A> Y;
    ap_int<B> U;
    ap_int<C> V;
  };

/* Line buffer class definition */
template <typename T, int LROW, int LCOL>
  class ap_linebuffer{
 public:
  T M[LROW][LCOL];

  ap_linebuffer(){
#pragma HLS ARRAY_PARTITION variable=M dim=1 complete
};
  ~ap_linebuffer(){};
  void shift_up(int col);
  void shift_down(int col);
  void shift_down();
  void insert_top(T value, int col);
  void insert_bottom(T value, int col);
  void insert_at(T value, int row, int col);
  T getval(int RowIndex,int ColIndex);
};

/* Line buffer shift up
 * Assumes new data pixel will be entered at the bottom of the line buffer
 * The bottom is row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_up(int col)
{
#pragma HLS inline
  int i;
  for(i = LROW-1; i > 0; i--){
#pragma HLS unroll
    M[i][col] = M[i-1][col];
  }
}

/* Line buffer shift down
 * Assumes new data pixel will be entered at the top of the line buffer
 * The bottom is row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_down(int col)
{
#pragma HLS inline
  int i;
  for(i = 0; i < LROW-1; i++){
#pragma HLS unroll
    M[i][col] = M[i+1][col];
  }
}
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::shift_down()
{
#pragma HLS inline
  int i, j;
  for(i = 0; i < LROW-1; i++){
    for(j = 0; j < LCOL; j++){
      #pragma HLS unroll
          M[i][j] = M[i+1][j];
    }
  }
}

/* Line buffer insert bottom
 * Inserts a new value in the bottom row of the line buffer at column = col
 * The bottom is row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_bottom(T value, int col)
{
#pragma HLS inline

  M[0][col] = value;
}

/* Line buffer insert top
 * Inserts a new value in the top row of the line buffer at column = col
 * The bottom is row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_top(T value, int col)
{
#pragma HLS inline

  M[LROW-1][col] = value;
}

template <typename T, int LROW, int LCOL>
  void ap_linebuffer<T,LROW,LCOL>::insert_at(T value, int row, int col)
{
#pragma HLS inline

    M[row][col] = value;
}

/* Line buffer getval
 * Returns the data value in the line buffer at position RowIndex, ColIndex
 */
template <typename T, int LROW, int LCOL>
  T ap_linebuffer<T,LROW,LCOL>::getval(int RowIndex,int ColIndex)
{
#pragma HLS inline

  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}

/* Memory window class definition */
template <typename T, int LROW, int LCOL>
  class ap_window{
 public:
  T M[LROW][LCOL];

  ap_window(){
#pragma HLS ARRAY_PARTITION variable=M dim=0 complete
    //#pragma HLS data_pack variable=M
};
  ~ap_window(){};
  void shift_right();
  void shift_left();
  void shift_up();
  void shift_down();
  void insert(T value, int row,int col);
  void rreflect();
  void lreflect();
  void ureflect();
  void dreflect();
  T getval(int RowIndex,int ColIndex);
};

/* Window shift right
 * Moves all the contents of the window horizontally
 * Assumes new values will be placed in column = LCOL-1
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_right()
{
#pragma HLS inline
  int i, j;
  for(i = 0; i < LROW; i++){
#pragma HLS unroll
    for(j=0; j < LCOL-1; j++){
#pragma HLS unroll
      M[i][j] = M[i][j+1];
    }
  }
}

/* Window shift left
 * Moves all the contents of the window horizontally
 * Assumes new values will be placed in column = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_left()
{
#pragma HLS inline
  int i, j;
  for(i = 0; i < LROW; i++){
#pragma HLS unroll
    for(j=LCOL-1; j > 0; j--){
#pragma HLS unroll
      M[i][j] = M[i][j-1];
    }
  }
}

/* Window shift up
 * Moves all the contents of the window vertically
 * Assumes new values will be placed in row = 0
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_up()
{
#pragma HLS inline
  int i, j;
  for(i = LROW-1; i > 0; i--){
#pragma HLS unroll
    for(j=0; j < LCOL; j++){
#pragma HLS unroll
      M[i][j] = M[i-1][j];
    }
  }
}

/* Window shift down
 * Moves all the contents of the window vertically
 * Assumes new values will be placed in row = LROW - 1
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::shift_down()
{
#pragma HLS inline
  int i, j;
  for(i = 0; i < LROW-1; i++){
#pragma HLS unroll
    for(j=0; j < LCOL; j++){
#pragma HLS unroll
      M[i][j] = M[i+1][j];
    }
  }
}

/* Window insert
 * Inserts a new value at any location of the window
 */
template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::insert(T value, int row, int col)
{
#pragma HLS inline
  M[row][col] = value;
}

template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::rreflect()
{
#pragma HLS inline
  int i;
  for(i = 0; i < LROW; i++){
#pragma HLS unroll
      M[i][0] = M[i][LCOL-1];
  }
}

template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::lreflect()
{
#pragma HLS inline
  int i;
  for(i = 0; i < LROW; i++){
#pragma HLS unroll
      M[i][LCOL-1] = M[i][0];
  }
}

template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::ureflect()
{
#pragma HLS inline
  int j;
  for(j = 0; j < LCOL; j++){
#pragma HLS unroll
      M[LROW-1][j] = M[0][j];
  }
}

template <typename T, int LROW, int LCOL>
  void ap_window<T,LROW,LCOL>::dreflect()
{
#pragma HLS inline
  int j;
  for(j = 0; j < LCOL; j++){
#pragma HLS unroll
      M[0][j] = M[LROW-1][j];
  }
}

/* Window getval
 * Returns the value of any window location
 */
template <typename T, int LROW, int LCOL>
  T ap_window<T,LROW,LCOL>::getval(int RowIndex, int ColIndex)
{
#pragma HLS inline
  T return_value;
  return_value = M[RowIndex][ColIndex];
  return return_value;
}

#endif
