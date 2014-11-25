#ifndef COMMON_H_
#define COMMON_H_

#define MAX_BYTE 256
#define MAX_CHARS MAX_BYTE
#define MAX_INDEX MAX_BYTE
#define echo_error(_x)       {std::cout << _x << std::endl;}

#define echo_msg_arg_error(_x)    {std::cout << "Incorrect number of arguments in" <<_x << std::endl;}
#define echo_msg_name_error(_x)   {std::cout << "expected msg "<<_x << " not received" << std::endl; }

#define echo_msg_conversion_error(_x,_y) {std::cout << "could not convert "<<_x <<" to " << _y;}

#define echo_data_type_error(_this_t_,_that_t_)  { \
    std::cout << "couldn't convert type " <<  _this_t_ << " to " << _that_t_ << std::endl; \
    }

#define echo_switch_nomatch_error(_fn_name_) {\
    std::cout << "no case match with the supplied value in switch statement with fun name: "<< " _fn_name " << std::endl; \
    }

#define MAX_ETHERNET_MSG_BYTES 512
#define VALID_MESSAGE_ID 12345
struct ethernetMessage{
    char msg[MAX_ETHERNET_MSG_BYTES];
    int id;
    int length;
};
#endif
