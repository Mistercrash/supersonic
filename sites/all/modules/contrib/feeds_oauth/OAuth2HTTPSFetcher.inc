<?php

/**
 * Definition of the import batch object created on the fetching stage by
 * OAuth2HTTPSFetcher.
 */
class OAuth2HTTPSBatch extends FeedsFetcherResult {
  protected $url;
  protected $authenticator;
  protected $consumer_key;
  protected $consumer_secret;
  protected $site_id;
  protected $uid;

  /**
   * Constructor.
   */
  public function __construct($url, $authenticator, $consumer_key, $consumer_secret, $site_id, $uid) {
    $this->url = $url;
    $this->authenticator = $authenticator;
    $this->consumer_key = $consumer_key;
    $this->consumer_secret = $consumer_secret;
    $this->site_id = $site_id;
    $this->uid = $uid;
    parent::__construct();
  }

  /**
   * Implementation of FeedsImportBatch::getRaw();
   */
  public function getRaw() {
    require_once(drupal_get_path('module', 'feeds_oauth') . '/php-proauth-read-only/lib/oauth/OAuth2Client.php');
    $access_token = call_user_func($this->authenticator, $this->uid, $this->site_id);
    $oauth = new OAuth2CurlClient();
    $oauth->_setAccessToken(new OAuth2AccessToken($access_token['oauth_token']));
    $request = $oauth->createGetRequest($this->url, array('access_token' => $oauth->getAccessToken()->getToken()));
    $response = $oauth->executeRequest($request);
    if ($response->getStatusCode() == 200) {
      return $response->getBody();
    }
    else {
      watchdog('feeds_oauth', print_r($response->getBody(), TRUE), array());
    }
  }
}


/**
 * Support OAuth 2.0 authentication.
 */
class OAuth2HTTPSFetcher extends FeedsHTTPFetcher {
  /**
   * Use signed URL to fetch content.
   */
  public function fetch(FeedsSource $source) {
    $source_config = $source->getConfigFor($this);
    $source_node = node_load($source->feed_nid);
    global $user;
    return new OAuth2HTTPSBatch(
      trim($source_config['source']),
      $this->config['authenticator'],
      $this->config['consumer_key'],
      $this->config['consumer_secret'],
      $this->config['site_id'],
      $source_node ? $source_node->uid : $user->uid
    );
  }

  /**
   * Declare defaults.
   */
  public function configDefaults() {
    return array(
      'username' => '',
      'password' => '',
      'developer_key' => '',
      'scope' => '',
    ) + parent::configDefaults();
  }

  /**
   * Add form options.
   */
  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    $form['username'] = array(
      '#type' => 'textfield',
      '#title' => t('Youtube username or linked Gmail email address'),
      '#default_value' => $this->config['username'],
      '#description' => t('Choose the username or email address you require to authenticate for this feed.'),
      '#required' => TRUE,
    );
    $form['password'] = array(
      '#type' => 'textfield',
      '#title' => t('Password'),
      '#default_value' => $this->config['password'],
      '#description' => t('The password for the youtube account.'),
      '#required' => TRUE,
    );
    $form['developer_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Developer Key'),
      '#default_value' => $this->config['developer_key'],
      '#required' => TRUE,
    );
    $form['scope'] = array(
      '#type' => 'textfield',
      '#title' => t('Scope'),
      '#description' => t('Scope of the authorization request.'),
      '#default_value' => $this->config['scope'],
    );
    return $form;
  }
}
