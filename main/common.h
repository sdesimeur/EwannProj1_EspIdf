
typedef struct {
    double x;
    double y;
    double z;
    double t;
} XYZT;


extern double magnetoField;
extern int magnetoFieldInit;

extern XYZT gyro;
extern XYZT accelI;
extern XYZT accelN;
extern XYZT accelP;
extern XYZT speed;
extern double Temp;


double get_speed_abs ();
double get_magnetofield ();
double get_magnetofieldMean ();
double get_magnetofieldMax ();
double get_magnetofieldMin ();
double get_accel_abs ();
void init_accel_speed();
