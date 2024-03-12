#include "usb_names.h"
#define GB_DEVICE_NAME   {'G','u','i','t','a','r','B','o','x'}
#define GB_DEVICE_NAME_LEN 9

// Do not change this part.  This exact format is required by USB.

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + GB_DEVICE_NAME_LEN * 2,
  3,
  GB_DEVICE_NAME
};
