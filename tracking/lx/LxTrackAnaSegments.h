#ifndef LXTRACKANASEGMENTS_INCLUDED
#define LXTRACKANASEGMENTS_INCLUDED

#pragma GCC diagnostic ignored "-Weffc++"

struct LxSimpleTrack;
class LxTrackAna;

class LxTrackAnaSegments
{
public:
  explicit LxTrackAnaSegments(LxTrackAna& o);
  void Init();
  void Finish();
  void BuildStatistics();

private:
  void StatForTrack(LxSimpleTrack* track);
  LxTrackAna& owner;
};

#endif//LXTRACKANASEGMENTS_INCLUDED
