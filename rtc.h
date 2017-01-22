
#define S3C2410_RTCCON 	0x40

#define S3C2410_RTCSEC  0x70
#define S3C2410_RTCMIN  0x74
#define S3C2410_RTCHOUR 0x78
#define S3C2410_RTCDATE 0x7C
#define S3C2410_RTCDAY  0x80
#define S3C2410_RTCMON  0x84
#define S3C2410_RTCYEAR 0x88

#define SUCCESS 0
#define DEV_NAME "myrtc"

struct rtc_time {
        int sec;
        int min;
        int hour;
        int day;
        int mon;
        int year;

};


