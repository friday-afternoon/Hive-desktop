#include "ThreadNet.h"

ThreadNet::ThreadNet(QObject *parent) : QThread(parent)
{
  udp_socket = new QUdpSocket(this);
  udp_socket->bind(udp_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
  connect(udp_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

  this->setParent(parent);
}

ThreadNet::~ThreadNet()
{
  status = false;
  emit usrLeft(&GlobalData::g_my_profile.key_str);
}

////////run
void ThreadNet::run()
{
  this->setPriority(QThread::NormalPriority);

  while(status)
    {
      refreshLocalHostIP();

      msleep(1000);
    }
}

void ThreadNet::setStatus(bool running)
{
  status = running;
}

void ThreadNet::refreshLocalHostIP()
{
  QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
  QHostAddress result;
  foreach(QHostAddress address, AddressList)
    {
      if(address.protocol() == QAbstractSocket::IPv4Protocol
         && address != QHostAddress::Null
         && address != QHostAddress::LocalHost)
        {
          if (!address.toString().contains("127.0."))
            {
              result = address;
              break;
            }
        }
    }

  if(!result.isNull())
    {
      GlobalData::g_localHostIP = result.toString();
    }
  else
    {
      GlobalData::g_localHostIP = "";
    }
  qDebug()<<"@refreshLocalHostIP(): finished!";
}

/////process packet
void ThreadNet::processMessage(MessageStruct *messageStruct)
{
  if(messageStruct->obj_key_str != GlobalData::g_my_profile.key_str)
    {
      if(messageStruct->subj_key_str != GlobalData::g_my_profile.key_str)
        {
          qDebug()<<"人家的事情我不管";
        }
      else
        {
          ////delete this!!2016-03-22
          QStringList message_str_list;
          message_str_list.append(messageStruct->obj_key_str);
          message_str_list.append(messageStruct->subj_key_str);
          message_str_list.append(messageStruct->message_str);
          message_str_list.append(GlobalData::g_currentTime());

          qDebug()<<"我发了消息："<<message_str_list[2];
          emit messageRecieved(message_str_list, true);
        }
    }
  else
    {
      if(messageStruct->subj_key_str == GlobalData::g_my_profile.key_str)
        {
          qDebug()<<"我发给自己的消息";
        }
      else
        {
          QStringList message_str_list;
          message_str_list.append(messageStruct->obj_key_str);
          message_str_list.append(messageStruct->subj_key_str);
          message_str_list.append(messageStruct->message_str);
          message_str_list.append(GlobalData::g_currentTime());
          qDebug()<<"别人发给我的："<<message_str_list.at(2);

          emit messageRecieved(message_str_list, false);
        }
    }
}

void ThreadNet::processUsrEnter(UsrProfileStruct *usrProfileStruct)
{


  if(usrProfileStruct->key_str == GlobalData::g_my_profile.key_str)
    {
      emit usrEnter(usrProfileStruct);

      qDebug()<<"UDP receive# Myself entered.";
    }

  qDebug()<<"UDP receive# Someone entered.";
  emit usrEnter(usrProfileStruct);
}

void ThreadNet::processUsrLeft(QString *usrKey)
{
  if(usrKey == GlobalData::g_my_profile.key_str)
    {
      emit usrLeft(usrKey);

      qDebug()<<"UDP receive# Myself left.";
    }

  qDebug()<<"UDP receive# Someone left.";
  emit usrLeft(usrKey);
}

void ThreadNet::processFileName()
{

}

void ThreadNet::processRefuse()
{

}


/////////////public slots////////////
void ThreadNet::sendUsrEnter()
{
  QByteArray data;
  QDataStream out(&data, QIODevice::WriteOnly);

  out << UsrEnter;
  out << GlobalData::g_localHostIP;
  out << GlobalData::g_my_profile.key_str;
  out << GlobalData::g_my_profile.name_str;
  out << GlobalData::g_my_profile.avatar_str;

  qint64 f = udp_socket->writeDatagram(data, data.length(), QHostAddress::Broadcast, udp_port);

  qDebug()<<"@sendUsrEnter(): finished!";
  return;
}

void ThreadNet::sendMessage(QString usrKeyStr, QString message)
{
  QByteArray data;
  QDataStream out(&data, QIODevice::WriteOnly);

  if(message == "")
    {
      qDebug()<<"@sendMessage(): Message content empty!";
      return;
    }

  out << Message << usrKeyStr << GlobalData::g_my_profile.key_str << message;
  udp_socket->writeDatagram(data,data.length(),QHostAddress::Broadcast, udp_port);
}

void ThreadNet::TEST_sendMessage(QString to, QString from, QString message)
{
  QByteArray data;
  QDataStream out(&data, QIODevice::WriteOnly);

  if (message == "")
    {
      qDebug()<<"@sendMessage(): Message content empty!";
      return;
    }

  out << Message << to << from << message;
  udp_socket->writeDatagram(data,data.length(),QHostAddress::Broadcast, udp_port);
}



/////////////private slots////////////

void ThreadNet::processPendingDatagrams()
{
  while(udp_socket->hasPendingDatagrams())
    {
      QByteArray datagram;
      datagram.resize(udp_socket->pendingDatagramSize());
      udp_socket->readDatagram(datagram.data(), datagram.size());
      QDataStream in(&datagram, QIODevice::ReadOnly);

      int msgType;
      in >> msgType;

      switch(msgType)
      {
        case Message:
          {
            MessageStruct message;
            in >> message.obj_key_str;
            in >> message.subj_key_str;
            in >> message.message_str;

            processMessage(&message);
            break;
          }
        case UsrEnter:
          {
            UsrProfileStruct usr_profile;
            in >> usr_profile.ip_str;
            in >> usr_profile.key_str;
            in >> usr_profile.name_str;
            in >> usr_profile.avatar_str;

            processUsrEnter(&usr_profile);
            break;
          }
        case UsrLeft:
          {

          }
        case Refuse:
          {

          }
        case FileName:
          {

          }
      }
    }
}
