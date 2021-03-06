/*
 * File Snake.cpp
 * created on 30-7-2018
 * by smit
 *
 * <smit17av@gmail.com>
 */

#include "snake.h"
#include "block.h"
#include <QKeyEvent>
#include <QTimer>
#include <QPoint>
#include "mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>


QSize *Snake::BlockSize;

Snake::Snake(QWidget *Parent,QSize ParentSize):
	QWidget(Parent),
	timer(new QTimer(this))
{
	BlockSize = new QSize(25,25);
	this->ParentSize = ParentSize;

	HeadStyle = new QString(
							"QFrame{"
							"border-image: url(\":/img/Head.png\");"
							"background-color: yellow;"
							"border-radius: 7%;"
							"}; "
						   );

	HitStyle = new QString(
							"QFrame{"
							"border-image: url(\":/img/cry.png\");"
							"min-height:30px;"
							"min-width:30px;"
							"}; "
						   );

	Pending.push(d);

	push(new Block(this));
	push(new Block(this));
	push(new Block(this));
	push(new Block(this));
	push(new Block(this));

	connect(timer,SIGNAL(timeout()),SLOT(Crawl()));
	timer->start(speed);

	setFocus();
	grabKeyboard();

}

void Snake::push(Block *b , Direction pd)
{
	QPoint p;
	if(!blocks.size())
	{
		b->move(130,130);
		blocks.push_back(b);
		return;
	}
	else
		p = blocks.front()->pos();
	QPoint des;
	switch (pd)
	{
		case LEFT:

			des.setX(p.x()-BlockSize->width()-1);
			des.setY(p.y());
			break;
		case RIGHT:
			des.setX(p.x()+BlockSize->width()+1);
			des.setY(p.y());
			break;
		case DOWN:
			des.setY(p.y()+BlockSize->width()+1);
			des.setX(p.x());
			break;
		case UP:
			des.setY(p.y()-BlockSize->width()-1);
			des.setX(p.x());
			break;
	}
	b->move(des);

	blocks.push_back(b);
}

void Snake::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_W:
		case Qt::Key_Up:
			if(d!=DOWN && Pending.front() != DOWN)
				d = UP;
			break;

		case Qt::Key_A:
		case Qt::Key_Left:
			if(d!=RIGHT && Pending.front() != RIGHT)
				d = LEFT;
			break;

		case Qt::Key_Down:
		case Qt::Key_S:
			if(d!=UP && Pending.front() != UP)
				d = DOWN;
			break;

		case Qt::Key_D:
		case Qt::Key_Right:
			if(d!=LEFT && Pending.front() != LEFT)
				d = RIGHT;
			break;
		case Qt::Key_P:
			timer->stop();
			break;
		case Qt::Key_R:
			timer->start(speed);
			break;
	}

	if(Pending.front() != d)
		Pending.push(d);

}


void Snake::Crawl()
{

	QPoint p = blocks.front()->pos();
	QPoint des;

	bool screencross = false;

	bool Maximizd = parentWidget()->windowState().testFlag(Qt::WindowMaximized);
	if(Maximizd)
	{
		if(p.x() > ParentSize.width())
		{
			des.setX(0);
			des.setY(p.y());
			screencross = true;
		}
		if(p.y() > ParentSize.height())
		{
			des.setY(0);
			des.setX(p.x());
			screencross = true;
		}

		if(!(p.x() > -1))
		{
			des.setX(ParentSize.width());
			des.setY(p.y());
			screencross = true;
		}
		if(!(p.y() > -1))
		{
			des.setY(ParentSize.height());
			des.setX(p.x());
			screencross = true;
		}
	}
	else
	{
		if(p.x() >  parentWidget()->geometry().width() )
		{
			des.setX(0);
			des.setY(p.y());
			screencross = true;
		}
		if(p.y() > parentWidget()->geometry().height())
		{
			des.setY(0);
			des.setX(p.x());
			screencross = true;
		}

		if(!(p.x() > -1))
		{
			des.setX(parentWidget()->width());
			des.setY(p.y());
			screencross = true;
		}
		if(!(p.y() > -1))
		{
			des.setY(parentWidget()->height());
			des.setX(p.x());
			screencross = true;
		}

	}	

	if(!screencross)
		switch (Pending.front())
		{
			case LEFT:
				des.setX(p.x()-BlockSize->width()-1);
				des.setY(p.y());
				break;
			case RIGHT:
				des.setX(p.x()+BlockSize->width()+1);
				des.setY(p.y());
				break;
			case DOWN:
				des.setY(p.y()+BlockSize->width()+1);
				des.setX(p.x());
				break;
			case UP:
				des.setY(p.y()-BlockSize->width()-1);
				des.setX(p.x());
				break;
		}


	if(Pending.size() > 1)
		Pending.pop();


	Block *temp = blocks.back();
	temp->setStyleSheet(*HeadStyle);
	blocks.front()->ResetStyle();
	temp->move(des);
	blocks.pop_back();
	blocks.insert(blocks.begin(),temp);


	if(isFruitReady)
	if(
	   (blocks.front()->pos().x() <= fruit->pos().x()+BlockSize->width() && blocks.front()->pos().x() >= fruit->pos().x()-BlockSize->width()) &&
	   (blocks.front()->pos().y() <= fruit->pos().y()+BlockSize->height() && blocks.front()->pos().y() >= fruit->pos().y()-BlockSize->height())
	  )
	{
		ft = new std::thread([&](){
			isFruitReady = false;
			fruit->Eaten();
			isFruitReady = true;
		});
		ft->detach();

		Block *New;
		for(int i = 0;i<28;i++)
		{
			New = new Block(this);
			New->move(blocks.back()->pos());
			PendingBlocks.push(New);
		}
	}


	if(!PendingBlocks.empty())
	{
		Block *tempB = PendingBlocks.front();
		tempB->move(blocks.back()->pos());
		blocks.push_back(tempB);
		PendingBlocks.pop();
	}

	if(!isFruitReady)
		fruit->hide();
	else if(isFruitReady)
		fruit->show();

	CheckHead();

}

void Snake::setFruit(Fruit *fruit)
{
	this->fruit = fruit;
}

void Snake::CheckHead()
{
	bool hited  = false;
	std::for_each(++blocks.begin(),blocks.end(), [&](Block *i)
	{
		if(blocks.front()->pos() == i->pos())
			hited = true;
	});

	if(hited)
	{

		blocks.front()->setStyleSheet(*HitStyle);
		blocks.front()->raise();
		timer->stop();

		QWidget *hit  = new QWidget();
		QGridLayout *grid = new QGridLayout();
		hit->setLayout(grid);
		QLabel *label = new QLabel("Opps ! You hit youself !!");
		QPushButton *Retry = new QPushButton("Retry");
		QPushButton *Quit  = new QPushButton("Exit");

		grid->addWidget(label,0,0,1,2);
		grid->addWidget(Retry,1,0);
		grid->addWidget(Quit,1,1);

		connect(Quit,SIGNAL(clicked()),QApplication::instance(),SLOT(quit()));
		connect(Retry,SIGNAL(clicked()),this,SLOT(Restart()));

		hit->setParent(this);
		hit->move(size().width()/2 - hit->size().width()/8 ,1);
		hit->setAutoFillBackground(true);
		hit->setWindowFlags(Qt::Widget);
		hit->show();
	}
}

void Snake::Restart()
{
	qApp->exit(11117);
}
