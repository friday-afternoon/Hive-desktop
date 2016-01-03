#include "GuiChatBubble.h"

GuiChatBubble_text_area::GuiChatBubble_text_area(QString text, int maxWidth, bool alignLeft = true, QWidget *parent)
{

  QFont font("Verdana");
  font.setPixelSize(14);

  QString message = "<p style=\"line-height:80%\">" + text + "</p>";
  label =  new QLabel(message);
  label->setFont(font);
  label->adjustSize();
  label->setWordWrap(true);
  label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label->setCursor(QCursor(Qt::IBeamCursor));

  main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(10,10,10,15);
  main_layout->addWidget(label);

  if(!alignLeft)
    {
      color = QColor(255,181,0);
      main_layout->setAlignment(Qt::AlignRight);
    }
  else
    {
      color = QColor(255,215,126);
      main_layout->setAlignment(Qt::AlignLeft);
    }

  this->setParent(parent);
  this->setFixedWidth(maxWidth);
  qDebug()<<label->width();
  qDebug()<<label->height();
}

void GuiChatBubble_text_area::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setPen(QPen(Qt::NoPen));
  //color options:
  //255,197,28,100
  //255,215,126
  painter.setBrush(QBrush(color,Qt::SolidPattern));
  painter.drawRoundedRect(label->x()-10,label->y()-10,label->rect().width()+20,label->rect().height()+20,12,12);
}


GuiChatBubble::GuiChatBubble(QString text, bool alignLeft = true, QWidget *parent) : QWidget(parent)
{
  if(alignLeft)
    {
      strip_pixmap.load("/Users/Echo/Desktop/asp.png");
    }
  else
    {
      strip_pixmap.load("/Users/Echo/Desktop/asp2.png");
    }

  strip_pixmap.setDevicePixelRatio(2.0);
  strip = new QLabel();
  strip->setPixmap(strip_pixmap);
  strip->setAlignment(Qt::AlignTop);
  strip->setContentsMargins(0,10,0,0);

  text_area = new GuiChatBubble_text_area(text, 400, alignLeft, this);

  main_layout = new QHBoxLayout(this);
  main_layout->setContentsMargins(0,0,0,0);
  main_layout->setSpacing(0);


  this->setParent(parent);

  if(alignLeft)
    {
      main_layout->setAlignment(Qt::AlignLeft);
      //main_layout->addWidget(strip);
      main_layout->addWidget(text_area);
    }
  else
    {
      main_layout->setAlignment(Qt::AlignRight);
      main_layout->addWidget(text_area);
      //main_layout->addWidget(strip);
    }
}