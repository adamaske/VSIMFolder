#ifndef VISUALPOINT_H
#define VISUALPOINT_H
#include "visualobject.h"
class VisualPoint : public VisualObject
{
public:
    VisualPoint();
    VisualPoint(const std::vector<Vertex>& v);
    ~VisualPoint() override;
    void init() override;
    void draw() override;

};
#endif // VISUALPOINT_H
