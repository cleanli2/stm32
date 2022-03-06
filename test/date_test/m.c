#include<stdio.h>
#include<date.h>
void pdf(date_info_t*dt,const char*mesg)
{
    printf("%s:\n%d.%02d.%02d %02d:%02d:%02d\n",
            mesg,
            dt->year,
            dt->month,
            dt->day,
            dt->hour,
            dt->minute,
            dt->second
          );
}
void main()
{
    date_info_t dt ={
        0, 2022, 5,1,7,2,3,3};
    date_info_t dt_lastadj={
        0, 2022, 4,6,9,2,3,3};
    printf("hello\n");
    pdf(&dt,"dt");
    pdf(&dt_lastadj, "dt_lastadj");
    printf("dt is later than dt_lastadj %d\n", is_later_than(&dt, &dt_lastadj));
    int diff_hours = time_diff_hours(&dt, &dt_lastadj);
    printf("diff hours %d\n", diff_hours);
    int diff_d = days_diff(&dt, &dt_lastadj);
    printf("diff days %d\n", diff_d);
}
