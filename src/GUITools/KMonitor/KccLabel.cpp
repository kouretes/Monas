#include "KccLabel.h"


KccLabel::KccLabel (QWidget *parent) : QLabel(parent) {
	// connect( this, SIGNAL( clicked() ), this, SLOT( slotClicked() ) );
}

void KccLabel::mousePressEvent(QMouseEvent *event) {
	emit clicked (event);
}
