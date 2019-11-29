package main

import (
  "crypto/hmac"
  "crypto/sha1"
  "crypto/tls"
  "crypto/x509"
  "fmt"
  "io/ioutil"
  "os"
  "time"
  "bytes"
  MQTT "github.com/eclipse/paho.mqtt.golang"
)

func main() {
  //mandatory input data
  var productKey string = "a1Zd7n5yTt8"
  var deviceName string = "deng"
  var deviceSecrete string = "UrwclDV33NaFSmk0JaBxNTqgSrJWXIkN"

  //optional input
  var timeStamp string = "1528018257135"
  var clientId string =  "192.168.56.1"
  var subTopic string = "/" + productKey + "/" + deviceName + "/user/get";
  var pubTopic string = "/" + productKey + "/" + deviceName + "/user/update";

  var raw_passwd bytes.Buffer
  raw_passwd.WriteString("clientId" + clientId)
  raw_passwd.WriteString("deviceName")
  raw_passwd.WriteString(deviceName)
  raw_passwd.WriteString("productKey")
  raw_passwd.WriteString(productKey);
  raw_passwd.WriteString("timestamp")
  raw_passwd.WriteString(timeStamp)
  fmt.Println(raw_passwd.String())
  //hmac ,use sha1
  mac := hmac.New(sha1.New, []byte(deviceSecrete))
  mac.Write([]byte(raw_passwd.String()))
  password := fmt.Sprintf("%02x", mac.Sum(nil))
  fmt.Println(password)

  var raw_broker bytes.Buffer
  raw_broker.WriteString("tls://")
  raw_broker.WriteString(productKey)
  raw_broker.WriteString(".iot-as-mqtt.cn-shanghai.aliyuncs.com:1883")
  opts := MQTT.NewClientOptions().AddBroker(raw_broker.String())

  var MQTTClientId bytes.Buffer
  MQTTClientId.WriteString(clientId)
  //hmac ,use sha1
  MQTTClientId.WriteString("|securemode=2,_v=sdk-c-3.0.1,lan=C,_ss=1,gw=0,ext=0,signmethod=hmacsha1,timestamp=") //TLS
  MQTTClientId.WriteString(timeStamp)
  MQTTClientId.WriteString("|")
  opts.SetClientID(MQTTClientId.String())

  opts.SetUsername(deviceName + "&" + productKey)   // device name and product key 
  opts.SetPassword(password)

  opts.SetKeepAlive(60 * 2 * time.Second)
  opts.SetDefaultPublishHandler(f)

  tlsconfig := NewTLSConfig()
  opts.SetTLSConfig(tlsconfig)
  //create and start a client using the above ClientOptions
  c := MQTT.NewClient(opts)

  if token := c.Connect(); token.Wait() && token.Error() != nil {
    panic(token.Error())
  }
  fmt.Print("Connect aliyun IoT Cloud Success\n");

  //subscribe to the topic /go-mqtt/sample and request messages to be delivered
  //at a maximum qos of zero, wait for the receipt to confirm the subscription
  if token := c.Subscribe(subTopic, 0, nil); token.Wait() && token.Error() != nil {
    fmt.Println(token.Error())
    os.Exit(1)
  }
  fmt.Print("Subscribe topic " + subTopic + " success\n");

  //Publish 5 messages to /go-mqtt/sample at qos 1 and wait for the receipt
  //from the server after sending each message
  for i := 0; i < 10; i++ {
    fmt.Println("publish msg:", i)
    text := fmt.Sprintf("ABC #%d", i)
    //    token := c.Publish("go-mqtt/sample", 0, false, text)
    token := c.Publish(pubTopic, 0, false, text)
    fmt.Println("publish msg: ", text)
    token.Wait()

    time.Sleep(20 * time.Second)
  }

  //unsubscribe from topic "/BDe33KsvGyu/rdtu001/get"
  if token := c.Unsubscribe(subTopic);token.Wait() && token.Error() != nil {
    fmt.Println(token.Error())
    os.Exit(1)
  }

  c.Disconnect(250)
}

func NewTLSConfig() *tls.Config {
  // Import trusted certificates from CAfile.pem.
  // Alternatively, manually add CA certificates to default openssl CA bundle.
  certpool := x509.NewCertPool()
  pemCerts, err := ioutil.ReadFile("./x509/root.pem")
  if err != nil {
    fmt.Println("0. read file error, game over!!")

  }

  certpool.AppendCertsFromPEM(pemCerts)

  // Create tls.Config with desired tls properties
  return &tls.Config{
    // RootCAs = certs used to verify server cert.
    RootCAs: certpool,
    // ClientAuth = whether to request cert from server.
    // Since the server is set up for SSL, this happens
    // anyways.
    ClientAuth: tls.NoClientCert,
    // ClientCAs = certs used to validate client cert.
    ClientCAs: nil,
    // InsecureSkipVerify = verify that cert contents
    // match server. IP matches what is in cert etc.
    InsecureSkipVerify: false,
    // Certificates = list of certs client sends to server.
    //    Certificates: []tls.Certificate{cert},
  }
}

//define a function for the default message handler
var f MQTT.MessageHandler = func(client MQTT.Client, msg MQTT.Message) {
  fmt.Printf("TOPIC: %s\n", msg.Topic())
  fmt.Printf("MSG: %s\n", msg.Payload())
}
