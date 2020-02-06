// -i inner_record.in inner_record.hh -- 
struct A {
    typedef struct _B {
        int x;
    } B;

    struct {
        int z;
    };

private:
    struct Private {
        int y;
    };
};
