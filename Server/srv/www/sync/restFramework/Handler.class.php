<?php 

abstract class RequestHandler
{
    private $m_returncode = 200;
    private $m_replyContentType = "octetstream/text";
    private $m_router;
    private $m_is_HEAD = false;
    
    public function requiresAuthentication()
    {
        return true;
    }
    
    public function postedObject()
    {
        $data = array_keys($_POST)[0];
        $data = base64_decode($data);
        return json_decode($data);
    }
    
    public function puttedObject()
    {
        $t = file_get_contents("php://input");
        return json_decode($t);
    }
        
    
    public function parentPath($path)
    {
        if($path[0] == "/")
        {
            $path = substr($path, 1);
        }

        if($path == "") 
        {   
            //We are given "/" ass $path and the parent of "/" is... well "/"
            return "/";
        }
        
        if($path[strlen($path)-1] == "/")
        {
            $path = substr($path, 0, strlen($path)-1);
        }
        
        $parts = explode("/", $path);
        array_pop($parts);
        if(sizeof($parts) == 0)
        {
            //We have reached the root...
            return "/";
        }
        $path = "/".implode("/", $parts)."/";
        return $path;
        
    }
    
    public function setIsHEAD($is_head)
    {
        $this->m_is_HEAD = $is_head;
    }
    
    public function isHead()
    {
        return $this->m_is_HEAD ;
    }
    
    public function setRouter($router)
    {
        $this->m_router = $router;
    }

    public function router()
    {
        return $this->m_router;
    }

    public function setReturnCode($code)
    {
        $this->m_returncode = $code;
    }

    public function returnCode()
    {
        return $this->m_returncode;
    }

    public function setReplyContentType($type)
    {
        $this->m_replyContentType = $type;
    }

    public function replyContentType()
    {
        return $this->m_replyContentType;
    }

    
    abstract function serialized();
    
    function sendAndDie()
    {
        http_response_code($this->m_returncode);
        header("Content-Type: ".$this->m_replyContentType);
        $s = $this->serialized();
        if($this->m_is_HEAD)
        {
            
            header("Content-Length: ".strlen($s));
            http_response_code(302);
        }
        else
        {
            echo $s;
        }
        die();        
    }

    
    /** Reads data and parses it as json into an object. Returns said object
    or thows a BadRequest exception*/
    public function PUTDataAsObject()
    {
        $f = fopen("php://input", "r");
        $data = "";
        while( ! feof($f) )
        {
            $data .= fread($f, 1024);
        }
        $v = json_decode($data);
        if($v === NULL)
        {
            throw new BadRequest(Error::PUTDataBadFormat, "json");
        }
        return $v;
    }
    

    /** Reads data and parses it as json into either a string or a file.*/
    public function loadPUTDataTo(&$dest)
    {
        if(is_string($dest))
        {
            $f = fopen("php://input", "r");
            $dest = "";
            while( ! feof($f) )
            {
                $dest .= fread($f, 1024);
            }
            return;
        }
        else if( is_resource($dest))
        {
            $f = fopen("php://input", "r");
            while( ! feof($f) )
            {
                fputs($dest, fread($f, 1024));
            }
            return;
        
        }
        throw new InternalServerError ("Provided destination is neither string nor a file descriptor.");
        
    }

}


class JsonReply extends RequestHandler
{
    private $m_variables = array();

    public function __construct()
    {
        $this->setReplyContentType("text/json");
    }
    
    function setReturnVariable($key, $value)
    {
        $this->m_variables[$key] = $value;
    }
    
    public function serialized()
    {
        $this->m_variables["success"] = ($this->returnCode() == 200);
        return json_encode($this->m_variables);
    }

}



/** This handler can return either a file or a constructed json document.
To make it a file that is returned, set the dataPath using setDataPath(). 
If you do not do that, it keeps onbeing a json handler.*/
class FileReply extends JsonReply
{

    private $m_baseName;
    private $m_dataPath = false;
    private $m_requestType = "GET";    
    
    function sendAndDie()
    {
        if($this->m_dataPath === false)
        {
            Parent::sendAndDie();
        }
        else
        {
            http_response_code($this->returnCode());
            foreach($this->headers() as $header)
            {
                header($header["key"].": ".$header["value"]);
            }
            
            if( $this->isHead())
            {
                http_response_code(302);                
                die();                
            }
            $f = fopen($this->m_dataPath, "r");
            fpassthru($f);
            fclose($f);
        }
        die();
    }
    
    function headers()
    {
        $h = array();
        $h[] = array("key" => "Content-Type", "value" => "application/octet-stream");
        $h[] = array("key" => 'Content-Disposition', "value" => 'attachment; filename="'.$this->m_baseName.'"');
        $h[] = array("key" => "Connection", "value" => "close");
        return $h;
    }
    
    
    /** Sets an lofal uri to the file that needs to be served, e.g.: /var/data/file-56.pdf */
    function setDataPath($name, $path)
    {
        $this->m_baseName = $name;
        $this->m_dataPath = $path;
    }
    
    function setRequestType($rqt)
    {
        $this->m_requestType = $rqt;
    }

}

?>