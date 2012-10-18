<?php 

class FeedsOAuth {
  
  public function __construct($consumer_key, $consumer_secret, $oauth_token = NULL, $oauth_token_secret = NULL) {
    $this->signature_method = new OAuthSignatureMethod_HMAC('SHA1');
    $this->consumer = new DrupalOAuthConsumer($consumer_key, $consumer_secret);
    if (!empty($oauth_token) && !empty($oauth_token_secret)) {
      $this->token = new DrupalOAuthConsumer($oauth_token, $oauth_token_secret);
    }
  }
  
  public function fetch_feed($feed_url, $options = array()) {
    $this->get($feed_url);
    //$this->oauth_request($feed_url);
  }
  
  /*
  public function oauth_request($url, $params = array(), $method = 'POST') {
    $request = OAuthRequest::from_consumer_and_token($this->consumer, $this->token, $method, $url, $params);
    $request->sign_request($this->signature_method, $this->consumer, $this->token);
    switch ($method) {
      case 'GET':
        return $this->request($request->to_url());
      case 'POST':
        return $this->request($request->get_normalized_http_url(), $request->get_parameters(), 'POST', $request);
    }
  }
  
  public function request($url, $params = array(), $method = 'POST', $request) {
    print ' - URL = ' . print_r($url, 1);
    print ' - params = ' . print_r($params, 1); 
    $data = '';
    if (count($params) > 0) {
      if ($method == 'GET') {
        $url .= '?'. http_build_query($params, '', '&');
      }
      else {
        $data = http_build_query($params, '', '&');
      }
    }

    $headers = array();

    $response = drupal_http_request($url, $headers, $method, $data);
    print '<pre>' . print_r($response, 1) . '</pre>';
    exit;
    if (!$response->error) {
      return $response->data;
    }
    else {
      $error = $response->error;
//      $data = $this->parse_response($response->data);
      if ($data['error']) {
        $error = $data['error'];
      }
    }
  }
  */
  
  public function get($path, $options = array()) {
    $options += array(
      'get' => FALSE,
      'params' => array(),
    );
    
    $token = $this->token;
    $req = OAuthRequest::from_consumer_and_token($this->consumer, $token,
      $options['get'] ? 'GET' : 'POST', $path, $options['params']);
    $req->sign_request($this->signature_method, $this->consumer, $token);

    $url = $req->get_normalized_http_url();
    $params = array();
    foreach ($req->get_parameters() as $param_key => $param_value) {
      if (substr($param_key, 0, 5) != 'oauth') {
        $params[$param_key] = $param_value;
      }
    }
    if (!empty($params)) {
      $url .= '?' . http_build_query($params);
    }
    
    $headers = array(
      $req->to_header(),
    );
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    if (!$options['get']) {
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, '');
    }
    $oauth_version = _oauth_common_version();
    curl_setopt($ch, CURLOPT_USERAGENT, 'Drupal/' . VERSION . ' OAuth/' . $oauth_version);
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_HEADER, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

    $response = curl_exec($ch);
    $error = curl_error($ch);
    curl_close($ch);

    if ($error) {
      throw new Exception($error);
    }

    $result = $this->interpretResponse($response);
    if ($result->responseCode != 200) {
      throw new Exception('Failed to fetch data from url "' . $path . '" (HTTP response code ' . $result->responseCode . ' ' . $result->responseMessage . '): ' . $result->body, $result->responseCode);
    }
    return $result->body;
  }
  
  protected function interpretResponse($res) {
    list($headers, $body) = preg_split('/\r\n\r\n/', $res, 2);

    $obj = (object) array(
      'headers' => $headers,
      'body' => $body,
    );

    $matches = array();
    if (preg_match('/HTTP\/1.\d (\d{3}) (.*)/', $headers, $matches)) {
      $obj->responseCode = trim($matches[1]);
      $obj->responseMessage = trim($matches[2]);

      // Handle HTTP/1.1 100 Continue
      if ($obj->responseCode == 100) {
        return $this->interpretResponse($body);
      }
    }

    return $obj;
  }
}