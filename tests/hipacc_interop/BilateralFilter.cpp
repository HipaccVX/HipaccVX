class BilateralFilter : public Kernel<uchar> {
private:
    Accessor<uchar> &in;
    Mask<float> &mask;
    Domain &dom;
    int sigma_r;

public:
    BilateralFilter(IterationSpace<uchar> &iter, Accessor<uchar> &in,
                    Domain &dom, Mask<float> &mask, int sigma_r)
          : Kernel(iter), in(in), mask(mask), dom(dom), sigma_r(sigma_r) {
        add_accessor(&in);
    }

    void kernel() {
        float c_r = 0.5f/(sigma_r*sigma_r);
        float d = 0.0f;
        float p = 0.0f;
        float center = in();

        iterate(dom, [&] () -> void {
                float diff = (float)(in(dom)) - center;
                float s = expf(-c_r * diff*diff) * mask(dom);
                d += s;
                p += s * in(dom);
            });

        output() = (uchar)(p/d+0.5f);
    }
};
