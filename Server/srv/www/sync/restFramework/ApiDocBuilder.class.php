<?php 

include "MarkDown/MarkdownExtra.inc.php";

use \Michelf\MarkdownExtra;



class Method 
{
    public $verb = "";
    public $doc = "";
}

class Uri 
{
    public $methods = array();
    public $anchor = "";
    public $description = "";
    public $uri = "";
}


function uriSorter($lhs, $rhs)
{
    return strcmp($lhs->uri, $rhs->uri);
}

class ApiDocBuilder
{

    private $m_uris = array();


    public function parse($handlers)
    {
    
        $i = 1;
        foreach($handlers as $url => $h)
        {
            $uri = new Uri();
            $uri->uri = preg_replace("/{[A-Z]+:([a-zA-Z0-9]*)}/","{\$1}",$url);
            $uri->anchor = preg_replace("/{[A-Z]+:([a-zA-Z0-9]*)}/","{\$1}",$url);

            foreach($h as $handler)
            {
            
                
                $i = $i + 1;
                
                $reflectionclass = new ReflectionClass($handler);
                $desc = trim($reflectionclass->getDocComment());
                $desc = substr($desc, 4);
                $desc = trim(substr($desc, 0, strlen($desc)-2));
                
                $uri->description .= "<p>$desc</p>";
                foreach(array("GET", "PUT", "DELETE", "UPDATE", "HEAD", "POST") as $method)
                {
                
                    if(  $reflectionclass->hasMethod($method) )
                    {
                        $m = new Method();
                        $m->verb = $method;
                        $rcm = $reflectionclass->getMethod($method);
                        if ( $comment = $rcm->getDocComment() )
                        {   
                            $comment = substr($comment, 4);
                            $comment = trim(substr($comment, 0, strlen($comment)-2));
                            
                            $m->doc = $comment;
                            $uri->methods[] = $m;
                        }
                    }
                }
            }
            
            $this->m_uris[] = $uri;
        }
        
        
        usort($this->m_uris, "uriSorter");
    }
    
    
    public function toHtml()
    {


        $css = <<<EOF
        
        
    <style type="text/css">
        
        table {
            border: 1px solid #3377dd;
            margin-bottom: 1em;
            border-collapse: collapse;
            border-radius: 1em;
        }
        
        td, th {
            padding-left: 0.5em;
            padding-right: 0.5em;
            background-color: #ffffff;
        }
        
        th {
            border-bottom: 1px solid #3377dd;
            text-align: left;
        }
        
        
        div.uri 
        {
            margin-left: 17em;
            min-width: 15em;
            max-width: 55em;
            padding-bottom: 2em;
        }
        
        div.uri h1  {
            color: #000;
            font-family: monospace;
        }
        
        div.uri p.uridesc  {
            color: #00ffff;
            background-color: #003300;
        }

        
        div.uri div.method
        {
            
            margin: 1em;
            padding-top: 1em;
            overflow: hidden;           
        }
        
        
        div.uri div.methodWrapper:nth-child(odd)
        {
            background-color:  #FFFFD6;
            border-radius: 0.5em;
        }
        
        div.uri div.methodWrapper:nth-child(even)
        {
            background-color:  #E6EEFB;
            border-radius: 0.5em;
        }

        div.uri div.methodWrapper pre 
        {
            border: 1px solid black;
            padding: 1em;
            
        }

    
        div.uri h2.name
        {
            margin-top: 0px;
            margin-bottom: 0px;
            font-size: 1em;
            color: #000;
            font-family: monospace;
            width : 5em;
            float: left;
        }

        div.uri div.doc
        {
            color: #000;
            float: left;
            min-width: 15em;
            max-width: 47em;
        }
        
        div.uri div.doc p
        {
            color: #000;
            margin-top: 0px;
        }
        
        
        /**  Index **/
        
        
        #index
        {
            position: fixed;
            top: 0px;
            left: 0px;
            height: 100%;
            width: 15em;
            background-color: #feffad;
            border-right: 1px solid #3377dd;
            overflow: auto;
            padding: 0px;
            margin: 0px;
        }
        
        #index h1
        {
            font-family: sans-serif;
        }
        
        #index a
        {
            font-family: monospace;
        }
        
        #index li
        {
            list-style-type: none;
            padding-bottom: 0.25em;
        }
        
        #index a
        {        
            text-decoration: none;
            color: #000;
        }
        
   
   
    
    </style>
        
        
EOF;
    
        $indexentries = array();
    
        $s = "<html><head></head>$css<body>";
        
        $s .= "<div id=\"index\"><ul><h1>Index</h1>";
        foreach($this->m_uris as $uri)
        {
            $s .= "<li>";
            $s .= "<a href=\"#".$uri->anchor."\">";
            $s .= $uri->uri;
            $s .= "</a>";
            $s .= "</li>";
        }
        $s .= "</ul></div>\n";

        foreach($this->m_uris as $uri)
        {
        
            $s .= "<div class=\"uri\">\n";
            $s .= "<h1><a name=\"".$uri->anchor."\">".$uri->uri."</a></h1>\n";
            $s .= "<p class=\"uridesc\">".MarkdownExtra::defaultTransform($uri->description)."</p>\n";
            foreach($uri->methods as $method)
            {
                
                $s .= "<div class=\"methodWrapper\">\n";
                
                $s .= "<div class=\"method\">\n";
                $s .= "<h2 class=\"name\">".$method->verb."</h2>\n";
                $s .= "<div class=\"doc\">\n";
                    
                $s .= MarkdownExtra::defaultTransform($method->doc);
                $s .= "</div> <!-- class=\"doc\" -->\n";
                $s .= "</div> <!-- class=\"method\" -->\n";
                $s .= "</div> <!-- class=\"methodWrapper\" -->\n";
            
            

            }
            $s .= "</div> <!-- class=\"uri\" -->\n";

        }
        return $s."</body></html>";        
    }

}

?>