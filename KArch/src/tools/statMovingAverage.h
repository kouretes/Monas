#ifndef  _statMovingAverage_h_
#define  _statMovingAverage_h_ 1

class StatMovingAverage {

    public:

        StatMovingAverage ( double alpha ) 
            : alpha(alpha), avg_x(0.0), avg_x_sqr(0.0)
        { ; } 

        StatMovingAverage () 
            : alpha(0.2), avg_x(0.0), avg_x_sqr(0.0)
        { ; } 

        void SetAlpha ( double alpha ) {
            this->alpha = alpha;
        }

        double GetAvg() const {
            return avg_x;
        }

        double GetVar() const {
            return avg_x_sqr - ( avg_x * avg_x );
        }

        double AddElement ( double x ) {

            avg_x = ( 1 - alpha ) * avg_x + alpha * x;

            avg_x_sqr = ( 1 - alpha ) * avg_x_sqr + alpha * x * x;

            return avg_x;
        }


    private:

        double alpha;

        double avg_x;

        double avg_x_sqr;
      
}; 

#endif // _statMovingAverage_h_
