namespace flens {

template <typename Mat, typename Vec,
          typename Res, typename Pro,
          typename S, typename DS>
MultiGrid<Mat, Vec, Res, Pro, S, DS>::MultiGrid(const Mat *_A, Vec *_f, Vec *_r,
                                                Vec *_u, DS &_ds)
    : A(_A), f(_f), r(_r), u(_u), ds(_ds)
{
}

template <typename Mat, typename Vec,
          typename Res, typename Pro,
          typename S, typename DS>
void
MultiGrid<Mat, Vec, Res, Pro, S, DS>::vCycle(int l, int v1, int v2)
{
    if (l==0) {
        ds.solve();
    } else {
        for (int v=1; v<=v1; ++v) {
            u[l] = S(A[l], f[l])*u[l];
        }
        r[l] = f[l] - A[l]*u[l];
        f[l-1] = R*r[l];

        u[l-1] = 0;
        vCycle(l-1,v1,v2);

        u[l] += P*u[l-1];
        for (int v=1; v<=v2; ++v) {
            u[l] = S(A[l], f[l])*u[l];
        }
    }
}

} // namespace flens
