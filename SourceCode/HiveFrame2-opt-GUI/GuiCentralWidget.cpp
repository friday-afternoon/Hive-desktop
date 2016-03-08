#include "GuiCentralWidget.h"

GuiCentralWidget::GuiCentralWidget(QWidget *parent) : QWidget(parent)
{

  ////data manager
  data_manager = new DataManager(this);


  ////net manager
  net_manager = new NetManager(this);


  ////Gui
  gui_tab_block = new GuiTabBlock(this);
  gui_main_block = new GuiMainBlock(this);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Plain);
  line->setFixedWidth(1);
  line->setStyleSheet ("QFrame{  background: #CFCFCF; border: transparent;  }");

  //main_layout
  main_layout = new QHBoxLayout(this);
  main_layout->setMargin(0);
  main_layout->setSpacing(0);
  main_layout->addWidget(gui_tab_block);
  main_layout->addWidget(line);
  main_layout->addWidget(gui_main_block);

  //connect
  connect(gui_tab_block->gui_chat_tab->comb_scroll_widget, SIGNAL(combWidgetClicked(QString)), gui_main_block, SLOT(displayChatStack(QString)));
  connect(gui_tab_block->gui_home_tab->welcome_btn, SIGNAL(clicked(StaticStackType)), gui_main_block, SLOT(displayStaticStack(StaticStackType)));
  connect(gui_tab_block->gui_home_tab->storage_btn, SIGNAL(clicked(StaticStackType)), gui_main_block, SLOT(displayStaticStack(StaticStackType)));
  connect(gui_tab_block->gui_settings_tab->messaging_btn, SIGNAL(clicked(StaticStackType)), gui_main_block, SLOT(displayStaticStack(StaticStackType)));
  connect(gui_tab_block->gui_settings_tab->profile_btn, SIGNAL(clicked(StaticStackType)), gui_main_block, SLOT(displayStaticStack(StaticStackType)));
  connect(gui_tab_block->gui_settings_tab->questions_btn, SIGNAL(clicked(StaticStackType)), gui_main_block, SLOT(displayStaticStack(StaticStackType)));


  ///data:
  connect(data_manager, SIGNAL(usrProfileLoaded(QStringList)), gui_tab_block->gui_chat_tab->comb_scroll_widget, SLOT(addComb(QStringList)));
  connect(data_manager, SIGNAL(usrProfileLoaded(QStringList)), gui_main_block, SLOT(addChatStack(QStringList)));

  data_manager->loadUsrProfile();



  ////net manager
  connect(net_manager, SIGNAL(messageRecieved(QStringList, bool)), gui_main_block, SLOT(onMessageRecieved(QStringList, bool)));

  foreach(GuiChatStack *temp_gui_chat_stack_pointer, gui_main_block->gui_chat_stack_map.values())
    {
      connect(temp_gui_chat_stack_pointer, SIGNAL(sendMessage(QString,QString)), net_manager, SLOT(sendMessage(QString,QString)));
    }

  ////threads
  thread_info = new ThreadInfo(this);

  connect(thread_info, SIGNAL(globalDataChanged()), data_manager, SLOT(writeCurrentConfig()));

  this->setParent(parent);
}

GuiCentralWidget::~GuiCentralWidget()
{
  thread_info->terminate();
  qDebug()<<"\n@Hive is destructed";
}
