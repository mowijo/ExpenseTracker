<?php 

include_once "Exceptions.class.php";
include_once "Handler.class.php";
include_once "ApiDocBuilder.class.php";


class Router
{

    private $m_handlers = array();
    private $m_enableApiDoc = false;
    private $m_authenticator = false;
    private $m_user = false;
    
    private $m_headers;
    
    
    
    public static function stringToDateTimeObject($string)
    {
        $format = "Y-m-d H:i:s";
        return DateTime::createFromFormat($format, $string, new DateTimeZone('UTC'));
    }
    
    public function requestHeader($name, $type = "text")
    {   
        if(array_key_exists($name, $this->m_headers))
        {
            $hv = $this->m_headers[$name];
            switch($type)
            {
                case "timestamp":
                    return $this->stringToDateTimeObject($hv);
                    break;
            
                case "text":
                default:
                    return $hv;
            }        
        }        
        return false;        
    }
    
    public function &user()
    {
        return $this->m_user;
    }
    
    public function setAuthenticator($authenticator)
    {
        $this->m_authenticator = $authenticator;
    }

    public function setApiDocEnabled($enabled)
    {
        $this->m_enableApiDoc = $enabled;
    }
    
    public function GET($expression, $handler)
    {
        $this->m_handlers[$expression] = $handler;
    }
    
    public function matches($url, $path)
    {
        $matches = array();

        //Get rid of any {N:id} like designations of parameters.
        $path = preg_replace("/{([A-Z]+):[a-zA-Z0-9]*}/","{\$1}",$path);

        $markers = array(
             "/{N}/" => "(\\d+)"            //Unsigned Integer
            ,"/{TEXT}/" => "([\\w-_\\.]+)"  //Latin, numbers and -, _ and .
            ,"/{EOL}/" => "(.*)"            //The rest of the line no matter what it may contain.
            ,"/{EMAIL}/" => "([\\w-_\\.\\@]+)" //An email address . Not the best regexp in the world...(shrug)
        );
        foreach($markers as $mark => $capture)
        {
            $path = preg_replace($mark, $capture, $path);
        }     
        $path = "^".$path."/?$";
        if ( ! preg_match("|".$path."|", $url, $matches) )
        {
            return false;
        }
        return array_slice($matches, 1);
    }
    
    public function addRoute($expression, $handler)
    {
        if( ! is_object($handler))
        {
            throw new InternalServerError("\"".$handler."\" is not a subclass of RequestHandler");
        }
        if( ! is_subclass_of ( $handler, "RequestHandler") )
        {
            throw new InternalServerError(get_class($handler)." is not a subclass of RequestHandler");
        }
        $handlers = array();
        if( array_key_exists ( $expression, $this->m_handlers) )
        {
            $handlers = $this->m_handlers[$expression];
        }
        $handlers[] = $handler;
        $this->m_handlers[$expression] = $handlers;
    }
    
   
    public function handle($url)
    {
    
        $this->m_headers = getallheaders();

    
        if( ($url == "/apidoc/" || $url == "/apidoc" ) && $this->m_enableApiDoc)
        {
            $adb  = new ApiDocBuilder();
            $adb->parse($this->m_handlers);
            echo $adb->toHtml();
            die();        
        }
    
        $verb = strtoupper($_SERVER["REQUEST_METHOD"]);
        

        
        foreach($this->m_handlers as $test => $handlers)
        {
            $mathcesurl = false;
            foreach($handlers as $handler)
            {
                $params = $this->matches($url, $test);
                if( is_array($params))
                {
                    $mathcesurl = true;
                    $reflectionclass = new ReflectionClass($handler);
                    if( ! $reflectionclass->hasMethod($verb) )
                    {
                        continue;
                    }
                    
                    $method = $reflectionclass->getMethod($verb);
                    $expected = $method->getNumberOfRequiredParameters ();
                    $found = sizeof($params);
                    if( $expected != $found)
                    {
                        throw new InternalServerError("Expected ".$expected." parameters but found ".$found." in url.");
                    }
                    if( $handler->requiresAuthentication() )
                    {
                        if($this->m_authenticator)
                        {
                            $a = $this->m_authenticator;
                            $this->m_user = $a();            
                        }
                    }                    
                    $handler->setRouter($this);
                    $method->invokeArgs ( $handler,$params);
                    $handler->sendAndDie();
                }
            }
            if( $mathcesurl )
            {
                throw new MethodNotImplementedException(Error::MethodNotImplemented, array($verb, $url));
            }
        }
        throw new FileNotFound();
    }
    
}

?>