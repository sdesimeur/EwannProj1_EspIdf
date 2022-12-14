
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
extern XYZT accelWG;
extern XYZT accelP;
extern XYZT speed;
extern double Temp;
extern unsigned int counter;
extern double time_in_s;
extern double counter_period;
extern double first_time;
extern double last_time;
extern int accelero_started;
extern int accel_speed_has_to_be_switch;
extern double accelMaxWG;
extern double magnetoField;
extern double magnetoFieldMean;
extern double magnetoFieldMax;
extern double magnetoFieldMin;
extern int GPIO_levels[];
extern int nb_of_doors;

double get_XYZT_abs(XYZT *a);
double get_magnetofield ();
double get_magnetofieldMean ();
double get_magnetofieldMax ();
double get_magnetofieldMin ();
void init_accel_speed();
void start_accelero(int a);
