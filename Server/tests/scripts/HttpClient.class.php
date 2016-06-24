<?php 


class HttpClient
{

    private $m_curl;
    
    
    private $m_hostname = "sync.data";
    
    private $m_user = "foo@bar.com";
    private $m_password = "foobar";
    
    private $m_protocol = "https";
    private $m_host = "127.0.0.1";
    private $m_port = 444;
    private $m_path;
    
    private $m_verbose = false;
    
    
    private $m_rawResponse = "";
    private $m_responseHeaders = array();
    private $m_responseCode;
    private $m_responseMessage;
    
    private $m_putData = false;
    private $m_putString = false;

    
    private $m_requestHeaders = array();

    public function __construct()
    {
    }


    public function setVerbose($verbose)
    {
        $this->m_verbose = $verbose;
    }
    
    public function setCredentials($username, $password)
    {
        $this->m_user = $username;
        $this->m_password = $password;    
    }
    
    public function url()
    {
        return $this->m_protocol."://".$this->m_host.$this->m_path;
    }
    
    
    public function parseResponseHeaderSection($headerchunk)
    {
        
        $headerchunk = trim(preg_replace("|HTTP/1.1 100 Continue|", "", $headerchunk));        
        
        $lines = explode("\n", $headerchunk);
        $line = trim($lines[0]);
        $lines = array_slice($lines, 1);
        
        $parts = explode(" ", $line);
        //$this->m_responseCode = intval($parts[1]);
        $parts = array_slice($parts, 2);
        $this->m_responseMessage = implode(" ", $parts);        
        
        $this->m_responseHeaders = $this->parseHeaderChunk(implode("\n", $lines));
        
    }
    
    public function addRequestHeader($name, $value)
    {
        $this->m_requestHeaders[$name] = $value;
    }
    
    public function parseHeaderChunk($headerchunk)
    {
        $headers = array();
        $lines = explode("\n", $headerchunk);
        foreach($lines as $line)
        {
            $line = trim($line);
            if($line == "") continue;

            $name = "";
            $i = 0;
            while($line[$i] != ":")
            {
                $name .= $line[$i];
                $i++;
            }
            
            $value = substr($line, $i+1);
            
            $name = trim($name);
            $value = trim($value);
            
            $headers[$name] = $value;
            
        }
        return $headers;
    
    }
    
    
    public function doGet($path)
    {
        return $this->performRequest("GET", $path);    
    }

    public function doHead($path)
    {
        return $this->performRequest("HEAD", $path);    
    }

    public function doDelete($path)
    {
        return $this->performRequest("DELETE", $path);    
    }
    
    public function doPut($path, $data)
    {
        $this->m_putData = json_encode($data);
        return $this->performRequest("PUT", $path);        
    }
    
    
    public function doPost($path, $data)
    {
        $this->m_putString = json_encode($data);
        return $this->performRequest("POST", $path);        
    }
    
    
    public function performRequest($verb, $path)
    {
        $this->m_path = $path;
        $ch = curl_init(); 

        // set url 
        curl_setopt($ch, CURLOPT_URL, $this->url()); 
        curl_setopt($ch, CURLOPT_PORT, intval($this->m_port)); 


        
        //return the transfer as a string 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
        curl_setopt($ch, CURLOPT_VERBOSE, ($this->m_verbose === true)  ? TRUE : FALSE);
        curl_setopt($ch, CURLOPT_HEADER, 1);
        curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $verb);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false); 
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false); 
        
        if($verb == "HEAD")
        {
            curl_setopt($ch, CURLOPT_NOBODY, true);         
        }        
        
        if($verb == "PUT")
        {
            if($this->m_putData !== false)
            {
                $tmpfname = tempnam(sys_get_temp_dir(), 'FOO'); 
                file_put_contents($tmpfname, $this->m_putData);
                curl_setopt($ch, CURLOPT_PUT, 1);

                $fh_res = fopen($tmpfname, 'r');

                curl_setopt($ch, CURLOPT_INFILE, $fh_res);
                curl_setopt($ch, CURLOPT_INFILESIZE, filesize($tmpfname));
                
                unlink($tmpfname);       
            }
        }
        
        if($verb == "POST")
        {        
            if($this->m_putString !== false)
            {
                $this->m_putString = base64_encode($this->m_putString);
                curl_setopt($ch, CURLOPT_POSTFIELDS,$this->m_putString);
                curl_setopt($ch, CURLOPT_HTTPHEADER, array(                                                                          
                    'Content-Length: ' . strlen($this->m_putString))                                                                       
                );  
            }
        }
        
        $this->addRequestHeader("Host", $this->m_hostname);
        
        $headers = array();
        foreach($this->m_requestHeaders as $key => $value)
        {
            $headers[] = $key.": ".$value;
        }
        
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        
        curl_setopt($ch, CURLOPT_USERPWD, $this->m_user.":".$this->m_password);  

        // $output contains the output string 

        
        $response = curl_exec($ch); 

            
        $header_size = curl_getinfo($ch, CURLINFO_HEADER_SIZE);
        $header = substr($response, 0, $header_size);
        $this->parseResponseHeaderSection($header);
        
        $this->m_rawResponse = substr($response, $header_size);
        // close curl resource to free up system resources 
        $this->m_responseCode = curl_getinfo ( $ch , CURLINFO_HTTP_CODE);
  
        if($this->m_verbose)
        {
            echo $this->m_rawResponse;
        }

        curl_close($ch);  
    }
    
    
    public function formatResponse($text, $type)
    {
        if(substr($type, 0, 9) == "text/json")
        {
            return json_decode($text);
        }
        return $text;
    
    }
    
    public function response()
    {
        if(isset($this->m_responseHeaders["Content-Type"]))
        {
            $type = $this->m_responseHeaders["Content-Type"];
            return $this->formatResponse($this->m_rawResponse, $type);
        }
        return $this->m_rawResponse;
    }

    public function rawResponse()
    {
        return $this->m_rawResponse;
    }

    public function responseHeaders()
    {
        return $this->m_responseHeaders ;
    }
    
    public function responseCode()
    {
        return $this->m_responseCode;
    }
    
    public function responseMessage()
    {
        return $this->m_responseMessage;
    }
}





function prettyPrintJson( $json )
{
    $result = '';
    $level = 0;
    $in_quotes = false;
    $in_escape = false;
    $ends_line_level = NULL;
    $json_length = strlen( $json );

    for( $i = 0; $i < $json_length; $i++ ) {
        $char = $json[$i];
        $new_line_level = NULL;
        $post = "";
        if( $ends_line_level !== NULL ) {
            $new_line_level = $ends_line_level;
            $ends_line_level = NULL;
        }
        if ( $in_escape ) {
            $in_escape = false;
        } else if( $char === '"' ) {
            $in_quotes = !$in_quotes;
        } else if( ! $in_quotes ) {
            switch( $char ) {
                case '}': case ']':
                    $level--;
                    $ends_line_level = NULL;
                    $new_line_level = $level;
                    break;

                case '{': case '[':
                    $level++;
                case ',':
                    $ends_line_level = $level;
                    break;

                case ':':
                    $post = " ";
                    break;

                case " ": case "\t": case "\n": case "\r":
                    $char = "";
                    $ends_line_level = $new_line_level;
                    $new_line_level = NULL;
                    break;
            }
        } else if ( $char === '\\' ) {
            $in_escape = true;
        }
        if( $new_line_level !== NULL ) {
            $result .= "\n".str_repeat( "\t", $new_line_level );
        }
        $result .= $char.$post;
    }

    return $result;
}
?>