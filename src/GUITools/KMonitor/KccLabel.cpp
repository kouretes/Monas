#include "KccLabel.h"
#include <iostream>

KccLabel::KccLabel (QWidget *parent) : QLabel(parent) {
	// connect( this, SIGNAL( clicked() ), this, SLOT( slotClicked() ) );
}

void KccLabel::mousePressEvent(QMouseEvent *event) {
	emit clicked (event);
}

void KccLabel::updateLabel(VisionDebugMessage vdm) {
	
	if(latestTimestamp - boost::posix_time::milliseconds(300) < boost::posix_time::from_iso_string(vdm.image_timestamp()) || 
		latestTimestamp + boost::posix_time::milliseconds(300) > boost::posix_time::from_iso_string(vdm.image_timestamp())) {
		
		latestImgLinesPix = latestImgPix;
		QPainter painter(&latestImgLinesPix);
		QPen pen(Qt::black);
		pen.setWidth(5);
		painter.setPen(pen);
	
		Polygon *poly;
		Point *p1, *p2;
		for(unsigned int p = 0 ; p < vdm.polygon_size() ; p++) {
			poly = vdm.mutable_polygon(p);
			for(unsigned int l = 0 ; l < poly->points_size() ; l++) {
				if( (l+1) != poly->points_size() ) {
					p1 = poly->mutable_points(l);
					p2 = poly->mutable_points(l+1);
				}
				else {
					p1 = poly->mutable_points(l);
					p2 = poly->mutable_points(0);
				}
				QLineF line(p1->x(), p1->y(), p2->x(), p2->y());
				painter.drawLine(line);
			}
		}
	
		if(vdm.has_ball_circle()) {
			QPen pen(Qt::blue);
			pen.setWidth(5);
			painter.setPen(pen);
			painter.drawEllipse(vdm.ball_circle().center().x(), vdm.ball_circle().center().y(), vdm.ball_circle().radius(), vdm.ball_circle().radius());
		}
	
		this->setPixmap(latestImgLinesPix);
		this->show();
    }
}
