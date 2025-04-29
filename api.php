<?php
/*

--
-- Structure de la table `TagPorte`
--

CREATE TABLE `TagPorte` (
  `pid` int(4) NOT NULL,
  `idtag` int(8) NOT NULL,
  `porte` int(1) NOT NULL,
  `prenom` varchar(20) NOT NULL,
  `nom` varchar(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

*/

echo controltag( Verif_magicquotes(M_filtre($_GET["tag"])), Verif_magicquotes(M_filtre($_GET["porte"])) );



function controltag($idtag, $porte) {
      $retour = sha1("ko".time()."valeur fausse");
      $bdd = new PDO('mysql:host=localhost;dbname=door;charset=utf8', 'identifiantdb', 'motdepassedb');
      $sql = "SELECT * FROM TagPorte where idtag = '".$idtag."'";
      $req = $bdd->prepare($sql);
      $req->execute();
      while ($ligne = $req->fetch()) {

        if ($ligne["porte"] < 3) {
          if ($porte == "WC02") { $portect = 1; }
          if ($porte == "WC01") { $portect = 2; }
        } else {
          $portect = 3;
        }

        if ($ligne["porte"] == $portect) {
          $retour = sha1($ligne["idtag"]."cledesecgms");
        }
      }

       if ($idtag <> "505505505") {
         $sql = "UPDATE TagPorteInconue SET idtag = '".$idtag."' WHERE pid = 1";
         $req = $bdd->prepare($sql);
         $req->execute();
       }

      return $retour;
}




function M_filtre($valtext) {
    if(is_array($valtext))
        return array_map(__METHOD__, $valtext);

    if(!empty($valtext) && is_string($valtext)) {
        return str_replace(array('\\', "\0", "\n", "\r", "'", '"', "\x1a"), array('\\\\', '\\0', '\\n', '\\r', "\\'", '\\"', '\\Z'), $valtext);
    }

    return $valtext;
}

function Verif_magicquotes ($chaine) {
    if (get_magic_quotes_gpc()) $chaine = stripslashes($chaine);
    return $chaine;
}

?>
