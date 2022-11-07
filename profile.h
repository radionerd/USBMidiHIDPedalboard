class PROFILE {
  private:
  unsigned long last_time_us;
  unsigned long max_duration_us;
  char * max_message;
  public:
  PROFILE(  ) { reset(); }
  
  unsigned long GetMaxDuration ( void ) { return max_duration_us ; }
  
  char * GetMaxMessage(void) { return max_message ; }

  void reset (void) { last_time_us = micros(); max_duration_us=0; max_message = "Reset"; }

  void log ( char * log_message ) {
    unsigned long time_us = micros();
    unsigned long duration_us = time_us - last_time_us ;
    last_time_us = time_us;
    if ( duration_us > max_duration_us ) {
      max_duration_us = duration_us;
      max_message = log_message ;
    }
  }
};
PROFILE profile;
