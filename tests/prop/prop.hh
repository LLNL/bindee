// -i prop.in prop.hh --

struct A {
    // normal
    int getReadWrite();
    void setReadWrite(int);
    int getReadOnly();
    void setWriteOnly(int);

    int get_read_write();
    void set_read_write(int);
    int get_read_only();
    void set_write_only(int);

    // short
    int get();
    void set(int);
    int get_();
    void set_(int);

};
