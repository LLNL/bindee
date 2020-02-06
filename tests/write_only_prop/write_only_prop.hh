// -i write_only_prop.in write_only_prop.hh --
struct Example {
    int getX();
    void setX(int);

    int getY();

    void setZ(int);

    int x,y,z;
};
