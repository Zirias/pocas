#include "bqt_window.h"
#include "bqt_backend.h"

SOLOCAL Bqt_Window::Bqt_Window(PG_Window *fw) : w(fw)
{

}

SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w)
{
    Bqt_Window *bw = new Bqt_Window(w);
    PG_qtBackend->privateApi->setBackendObject(w, bw);
    return 1;
}
