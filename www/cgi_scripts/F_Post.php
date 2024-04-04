<?php

$databaseFile = 'idk1.db';
$pdo = new PDO('sqlite:' . $databaseFile);
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);


$createTableQuery = "
    CREATE TABLE IF NOT EXISTS idk1 (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        task TEXT NOT NULL
    )
";
$pdo->exec($createTableQuery);


if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["task"])) {
    $task = $_POST["task"];
    $insertQuery = "INSERT INTO idk1 (task) VALUES (:task)";
    $stmt = $pdo->prepare($insertQuery);
    $stmt->bindParam(':task', $task, PDO::PARAM_STR);
    $stmt->execute();
}


if (isset($_GET["delete"]) && is_numeric($_GET["delete"])) {
    $deleteId = $_GET["delete"];
    $deleteQuery = "DELETE FROM idk1 WHERE id = :id";
    $stmt = $pdo->prepare($deleteQuery);
    $stmt->bindParam(':id', $deleteId, PDO::PARAM_INT);
    $stmt->execute();
}


$selectQuery = "SELECT * FROM idk1";
$idk1 = $pdo->query($selectQuery)->fetchAll(PDO::FETCH_ASSOC);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple idk2 App</title>
</head>
<body>

<h2>idk2 App</h2>


<form method="post" action="/www/cgi_scripts/F_Post.php">
    <label for="task">New Task:</label>
    <input type="text" name="task" required>
    <button type="submit">Add Task</button>
</form>


<ul>
    <?php foreach ($idk1 as $idk2): ?>
        <li>
            <?php echo htmlspecialchars($idk2["task"]); ?>
            <a href="?delete=<?php echo $idk2["id"]; ?>" onclick="return confirm('Are you sure?')">Delete</a>
        </li>
    <?php endforeach; ?>
</ul>

</body>
</html>