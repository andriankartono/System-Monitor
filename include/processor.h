#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  long NewTotal() const;
  long NewIdle() const;
  long NewActive() const;
  void Update(long total, long active, long idle);
  long OldTotal() const;
  long OldIdle() const;
  long OldActive() const;

  // TODO: Declare any necessary private members
 private:
 long idle_;
 long active_;
 long total_;
};

#endif