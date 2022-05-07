#ifndef _PIXY2LINE_H
#define _PIXY2LINE_H

#include "pixy.h"

#define LINE_REQUEST_GET_FEATURES                0x30
#define LINE_RESPONSE_GET_FEATURES               0x31
#define LINE_REQUEST_SET_MODE                    0x36
#define LINE_REQUEST_SET_VECTOR                  0x38
#define LINE_REQUEST_SET_NEXT_TURN_ANGLE         0x3a
#define LINE_REQUEST_SET_DEFAULT_TURN_ANGLE      0x3c
#define LINE_REQUEST_REVERSE_VECTOR              0x3e

#define LINE_GET_MAIN_FEATURES                   0x00
#define LINE_GET_ALL_FEATURES                    0x01

#define LINE_MODE_TURN_DELAYED                   0x01
#define LINE_MODE_MANUAL_SELECT_VECTOR           0x02
#define LINE_MODE_WHITE_LINE                     0x80

// features
#define LINE_VECTOR                              0x01
#define LINE_INTERSECTION                        0x02
#define LINE_BARCODE                             0x04
#define LINE_ALL_FEATURES                        (LINE_VECTOR | LINE_INTERSECTION | LINE_BARCODE)

#define LINE_FLAG_INVALID                        0x02
#define LINE_FLAG_INTERSECTION_PRESENT           0x04

#define LINE_MAX_INTERSECTION_LINES              6

//responses
#define PIXY_TYPE_RESPONSE_RESULT            0x01

/**
* Feature definitions
**/

struct Vector
{
  void print()
  {
    printf("vector: (%d %d) (%d %d) index: %d flags %d", m_x0, m_y0, m_x1, m_y1, m_index, m_flags);
  }
  
  uint8_t m_x0;
  uint8_t m_y0;
  uint8_t m_x1;
  uint8_t m_y1;
  uint8_t m_index;
  uint8_t m_flags;
};


struct IntersectionLine
{
  uint8_t m_index;
  uint8_t m_reserved;
  int16_t m_angle;
};

struct Intersection
{
  void print()
  {
    uint8_t i;
    printf("intersection: (%d %d)", m_x, m_y);
    for (i=0; i<m_n; i++)
    {
      printf("  %d: index: %d angle: %d", i, m_intLines[i].m_index, m_intLines[i].m_angle);;
    }
  }
  
  uint8_t m_x;
  uint8_t m_y;
	
  uint8_t m_n;
  uint8_t m_reserved;
  IntersectionLine m_intLines[LINE_MAX_INTERSECTION_LINES];
};

struct Barcode
{
  void print()
  {
    printf("Barcode: (%d %d), val: %d flags: %d", m_x, m_y, m_code, m_flags);
  }
  
  uint8_t m_x;
  uint8_t m_y;
  uint8_t m_flags;
  uint8_t m_code;
};


/**
* Feature communication
**/

class Pixy2Features {
public:
    Pixy2Features(Pixy2 pixy)
    {
        m_pixy = pixy;
    }
    Pixy2Features() {
        m_pixy = Pixy2();
    }
    
    int8_t getMainFeatures(uint8_t features=LINE_ALL_FEATURES, bool wait=true)
    {
        printf("Getting main features\n");
        return getFeatures(LINE_GET_MAIN_FEATURES, features, wait); 
    }
    
    int8_t getAllFeatures(uint8_t features=LINE_ALL_FEATURES, bool wait=true)
    {
        return getFeatures(LINE_GET_ALL_FEATURES, features, wait);   
    }
    
    int8_t setMode(uint8_t mode);
    int8_t setNextTurn(int16_t angle);
    int8_t setDefaultTurn(int16_t angle);
    int8_t setVector(uint8_t index);
    int8_t reverseVector();
    
    uint8_t numVectors;
    Vector *vectors;
    
    uint8_t numIntersections;
    Intersection *intersections;

    uint8_t numBarcodes;
    Barcode *barcodes;

private:
    int8_t getFeatures(uint8_t type, uint8_t features, bool wait);
    Pixy2 m_pixy;
    
};

/**
* @brief A function to get all features (vectors, barcodes, intersections), from the Pixy2.
* @param type
* @param features
* @param wait
* @return 
**/
int8_t Pixy2Features::getFeatures(uint8_t type,  uint8_t features, bool wait)
{
  int8_t res;
  uint8_t offset, fsize, ftype, *fdata;
  uint8_t data[1024];
  
  vectors = NULL;
  numVectors = 0;
  intersections = NULL;
  numIntersections = 0;
  barcodes = NULL;
  numBarcodes = 0;
  
  while(1)
  {
    // fill in request data
    uint8_t msg[2] = {type, features};
    m_pixy.prepare_msg(LINE_REQUEST_GET_FEATURES, 2, msg);
 
    // send request
    m_pixy.send_msg();
    if (m_pixy.recv_msg(data)==0)
    {     
      if (m_pixy.get_type()==LINE_RESPONSE_GET_FEATURES)
      {
        // parse line response
		for (offset=0, res=0; m_pixy.get_len()>offset; offset+=fsize+2)
        {
          fsize = data[offset+1];
          ftype = data[offset];
          fdata = &data[offset+2]; 
          if (ftype==LINE_VECTOR)
          {
            vectors = (Vector *)fdata;
            numVectors = fsize/sizeof(Vector);
            res |= LINE_VECTOR;
		      }
		      else if (ftype==LINE_INTERSECTION)
          {
            intersections = (Intersection *)fdata;
            numIntersections = fsize/sizeof(Intersection);
            res |= LINE_INTERSECTION;
          }
 		      else if (ftype==LINE_BARCODE)
          {
            barcodes = (Barcode *)fdata;
            numBarcodes = fsize/sizeof(Barcode);;
            res |= LINE_BARCODE;
          }
          else
            break; // parse error
        }
        return res;
      }
      else if (m_pixy.get_type()==PIXY_RESULT_RESPONSE_ERROR)
      {
		    // if it's not a busy response, return the error
        if ((int8_t)data[0]!=PIXY_RESULT_BUSY)
		      return data[0];
	    else if (!wait) // we're busy
          return PIXY_RESULT_BUSY; // new data not available yet
      }
    }
    else
        printf("ERROR\n");
        return PIXY_RESULT_ERROR;  // some kind of bitstream error
    
        // If we're waiting for frame data, don't thrash Pixy with requests.
        // We can give up half a millisecond of latency (worst case)	
        thread_sleep_for(500);
  }
}

int8_t Pixy2Features::setMode(uint8_t mode)
{
  uint32_t res;
  uint8_t recvbuffer[1024];
  uint8_t buf[1];
  buf[0] = mode;
  m_pixy.prepare_msg(LINE_REQUEST_SET_MODE, 1, &(buf[0]));
  m_pixy.send_msg();
  if (m_pixy.recv_msg(recvbuffer)!=0 && m_pixy.get_type()==PIXY_TYPE_RESPONSE_RESULT && m_pixy.get_len()==4)
  {
    res = *(uint32_t *)recvbuffer;
    return (int8_t)res;	
  }
  else
      printf("ERROR::PIXY2FEATURE::setMODE \n\r");
      return PIXY_RESULT_ERROR;  // some kind of bitstream error
}

#endif /* _PIXY2LINE_H */
