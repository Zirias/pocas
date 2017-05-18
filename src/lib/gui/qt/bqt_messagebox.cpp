#include <QMessageBox>

#include "bqt_backend.h"
#include "bqt_window.h"

SOLOCAL_CDECL PG_MessageBoxButton Bqt_MessageBox_show(const PG_Window *w, const char *title,
        const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style)
{
    Bqt_Window *pw = w ? (Bqt_Window *)PG_qtBackend->privateApi->backendObject(w) : 0;
    QWidget *parent = pw ? pw->widget() : 0;

    QMessageBox::StandardButtons qb = QMessageBox::NoButton;
    if (buttons & PG_MBB_Again) qb |= QMessageBox::Abort;
    if (buttons & PG_MBB_Cancel) qb |= QMessageBox::Cancel;
    if (buttons & PG_MBB_Continue) qb |= QMessageBox::Ignore;
    if (buttons & PG_MBB_Help) qb |= QMessageBox::Help;
    if (buttons & PG_MBB_No) qb |= QMessageBox::No;
    if (buttons & PG_MBB_OK) qb |= QMessageBox::Ok;
    if (buttons & PG_MBB_Retry) qb |= QMessageBox::Retry;
    if (buttons & PG_MBB_Yes) qb |= QMessageBox::Yes;

    QMessageBox::StandardButton qr;

    switch (style)
    {
    case PG_MBS_Warning:
        qr = QMessageBox::warning(parent, title, text, qb);
        break;
    case PG_MBS_Error:
        qr = QMessageBox::critical(parent, title, text, qb);
        break;
    case PG_MBS_Question:
        qr = QMessageBox::question(parent, title, text, qb);
        break;
    case PG_MBS_None:
    case PG_MBS_Info:
    default:
        qr = QMessageBox::information(parent, title, text, qb);
        break;
    }

    switch (qr)
    {
    case QMessageBox::Abort: return PG_MBB_Again;
    case QMessageBox::Cancel: return PG_MBB_Cancel;
    case QMessageBox::Ignore: return PG_MBB_Continue;
    case QMessageBox::Help: return PG_MBB_Help;
    case QMessageBox::No: return PG_MBB_No;
    case QMessageBox::Ok: return PG_MBB_OK;
    case QMessageBox::Retry: return PG_MBB_Retry;
    case QMessageBox::Yes: return PG_MBB_Yes;
    default: return PG_MBB_Unsupported;
    }
}
